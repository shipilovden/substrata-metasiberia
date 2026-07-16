# Инвентарь и экипировка

Назначение: каноническое описание native Gear Inventory, avatar-and-gear preview, клиент-серверной синхронизации и границ проверки.

Состояние на 2026-07-17: реализация находится в active working tree. Локальные Windows-сборки `gui_client` и `server` прошли compile/link. По прямому требованию владельца GUI-клиент после изменений не запускался, поэтому визуальный результат и полный пользовательский сценарий не объявляются проверенными. Production Linux server не обновлялся; deploy требует отдельного явного разрешения.

## Native Qt UI

- `GearInventoryPanel.*` размещён в общем левом `editorDockWidget`. Панель показывает экипированные и доступные предметы, карточки с preview/именем/bone/model/UID, точные transform-поля и действия с packaged Lucide SVG icons.
- `AvatarGearPreviewWidget` наследует `AvatarPreviewWidget`. Это не снимок основного мира и не специальный режим world renderer.
- Как и окно настройки аватара, preview является отдельным Qt OpenGL widget со своим OpenGL context, собственным `TextureServer` и собственным `OpenGLEngine`. Он использует ту же инициализацию preview renderer, небо, ground plane, перспективную камеру и базовые параметры вида, что `Avatar Settings`.
- Наследование сохраняет то же управление камерой: ЛКМ вращает вид, ПКМ/средняя кнопка перемещает цель камеры, колесо меняет масштаб. В режимах Move/Rotate/Scale ЛКМ передаётся редактору выбранного gear item, но zoom колёсиком остаётся доступен.
- Загрузка avatar mesh, Idle-анимации и grounding повторяет путь `AvatarSettingsWidget`: сначала вычисляется исходная опора, затем выполняется retarget Idle и повторно вычисляется положение ступней. Итоговая модель ставится ступнями на ground plane; портретное соотношение сторон сохраняет голову и ноги в кадре.
- Preview загружает текущие `AvatarSettings` и `GearItems` в свой engine. Каждый предмет прикрепляется к animated bone текущего preview-avatar через `bone_name` и `gearObToBoneSpaceMatrix()`.
- `PreviewContextScope` гарантированно возвращает основной GL context после инициализации, перестройки и shutdown preview. Preview не обращается к world `OpenGLEngine`, не рисует world scene в свой framebuffer, не меняет world camera и не включает third-person. Поэтому открытие окна экипировки не должно влиять на рендер мира, desktop camera или XR path.
- Пока widget видим, таймер обновляет анимированный кадр примерно каждые 33 мс. Тяжёлая перестройка avatar/gear GL objects выполняется только при изменении данных или появлении недостающего ресурса; доступность ожидаемых ресурсов перепроверяется с ограниченной частотой.
- Legacy `GearInventoryUI` остаётся альтернативным non-Qt UI path и получает те же all/equipped caches. Его SDL/Web parity в текущем проходе не проверялась.

## Ресурсы preview

- `GUIClient::requestGearPreviewResources()` формирует временный `Avatar` из `logged_in_avatar_settings` и `logged_in_equipped_gear` и запрашивает его зависимости через обычный `ResourceManager` pipeline. Card thumbnails из `logged_in_all_gear` запрашиваются отдельно.
- `AvatarGearPreviewWidget` сначала разрешает LOD0 optimized model URL в соответствии с capability сервера, затем использует исходный URL как fallback. Для текстур применяются те же Basis/original flags, которые клиент получил от сервера.
- Отсутствующий mesh не заменяется объектом из мировой сцены: widget ждёт ресурс и перестраивает только собственную preview scene. Resource request сам по себе не меняет world render или режим камеры.

## Графика экипировки в мире

- `AvatarGraphics::equipped_gear_graphics` хранит отдельный GL object, gear UID, bone name/index и gear-to-bone transform для каждого экипированного предмета.
- `GUIClient::loadGearModelsForAvatar()` загружает model/material/texture dependencies через существующий async avatar resource pipeline.
- `AvatarGraphics::updateGearBones()` разрешает bone indices, а `AvatarGraphics::setOverallTransform()` применяет animated bone matrix и transform предмета. Этот мировой путь отделён от `AvatarGearPreviewWidget` и не используется для получения preview.

## Клиентская синхронизация

- Поддержка определяется capability `Protocol::GEAR_INVENTORY_SUPPORT`, а не только общей protocol version. Без capability клиент не отправляет `QueryUserGear`, `CreateGearItem` или `GearItemUpdate` и не пытается изменить equipped state через несовместимый сервер.
- После login клиент принимает server-provided `AvatarSettings` и equipped gear, очищает прежний полный inventory cache, запрашивает `UserGearList` и обновляет обе UI-реализации. Logout и disconnect очищают avatar/gear caches и capability state.
- Серверный `AvatarFullUpdate` собственного аватара переносится из `ClientThread` в GUI thread как `OurAvatarFullUpdateMessage`. GUI принимает одновременно authoritative `AvatarSettings` и `GearItems`, после чего обновляет preview и панели. Это не позволяет локальному preview или world transition сохранять устаревшую модель либо отфильтрованную сервером экипировку.
- Equip/unequip разрешены только при активном соединении, login, capability и существующем собственном avatar в текущем мире. Equip дополнительно требует, чтобы UID находился в полученном `logged_in_all_gear`. Локальный cache меняется только вместе с подготовкой и отправкой `AvatarFullUpdate`; при отсутствии avatar операция отклоняется без локального рассинхрона.
- `updateOurAvatarModel()` включает текущую экипировку в `AvatarFullUpdate`, сразу обновляет локальный cache уже преобразованных `AvatarSettings`, а затем принимает authoritative echo сервера. `CreateAvatar` при переходе между мирами также включает cached avatar settings и gear; сервер всё равно восстанавливает gear из собственного user state.

## Authoritative server contract

Wire contract gear существует с protocol version 52. Текущий `WorkerThread` реализует:

- `QueryUserGear` → `UserGearList` из `User::gear_ids` и server-owned `gear_items`;
- `CreateGearItem` → проверка login/read-only/model/transform, выдача server UID, принудительные owner/creator, создание pending gear screenshot, обновление inventory и DB dirty state, ответ новым `UserGearList`, запрос resource/LOD dependencies;
- `GearItemUpdate` → проверка finite и ограниченных translation/axis/angle/scale, read-only state, наличия UID в inventory и совпадения owner; server flags не принимаются от клиента, успешное изменение помечается DB dirty и распространяется через avatar update;
- `AvatarFullUpdate` → проверка UID собственного аватара, фильтрация requested gear по `User::gear_ids` и owner, удаление повторов, ограничение списка 24 предметами и замена клиентских объектов authoritative `GearItem` references;
- `CreateAvatar` после подключения или перехода мира → восстановление экипировки из server-owned `User::equipped_gear_ids`, а не доверие полям gear из клиентского payload.

При принятом `AvatarFullUpdate` сервер сохраняет `AvatarSettings` и authoritative equipped IDs в `User`, помечает пользователя DB dirty и сразу возвращает отправителю authoritative `AvatarFullUpdate`. В read-only mode сервер сохраняет прежнюю экипировку и также не принимает mutation. Login responses для protocol 52+ содержат authoritative equipped gear.

`User` serialization version 7 хранит `gear_ids` и `equipped_gear_ids`; сами предметы остаются server-owned записями `gear_items`. Поэтому reconnect/login и world transition восстанавливают оборудование из server persistence, а не из локальной панели.

Capability разрешено публиковать только бинарю, в котором одновременно присутствуют все handlers, login/equipped serialization, ownership validation и persistence. После Linux-сборки и выкладки 2026-07-17 production `metasiberia-server` работает на новом ELF `server` и публикует `GEAR_INVENTORY_SUPPORT`.

## Проверки и production boundary

Подтверждено локально на Windows 2026-07-17:

- `gui_client` compile/link прошёл;
- `server` compile/link прошёл;
- статическая сверка client/server message IDs, capability, ownership validation и persistence path выполнена.

По требованию владельца `gui_client.exe` не запускался. Поэтому не подтверждены: внешний вид и управление реального Qt окна, live create/equip/unequip/edit, reconnect, переход между мирами, second-client visibility, screenshot-bot preview generation и SDL/Web parity.

Production `vr.metasiberia.com` обновлён 2026-07-17: release `/srv/metasiberia/releases/gear-20260716_182028/server` переключён в `/srv/metasiberia/releases/current`, `metasiberia-server.service` перезапущен и находится в состоянии `active/running`. Windows `server.exe` не является production artifact; production использует Linux ELF `server` и workflow из `docs/SERVERS_AND_EXCHANGE.md`.

## Ключевые файлы

- Qt UI/preview: `gui_client/GearInventoryPanel.*`, `gui_client/AvatarGearPreviewWidget.*`, `gui_client/AvatarPreviewWidget.*`.
- Client state/render: `gui_client/GUIClient.*`, `gui_client/ClientThread.*`, `gui_client/AvatarGraphics.*`.
- Shared contract/data: `shared/Protocol.h`, `shared/GearItem.*`, `shared/Avatar.*`.
- Server validation/persistence: `server/WorkerThread.cpp`, `server/User.*`, server world-state gear storage.
