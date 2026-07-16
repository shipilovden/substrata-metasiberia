# Metasiberia Beta v0.0.21

Дата: 2026-05-27

## Главное

Этот релиз доводит до полноценного рабочего состояния редактор ботов и закрывает несколько важных игровых направлений вокруг AI-персонажей, звука, жестов, снаряжения, карт, порталов и XR-режима.

## Редактор ботов

1. Добавлен полноценный dock-редактор ботов в клиенте: список ботов, выбор текущего бота, удаление, сохранение, отмена, редактирование позиции, поворота, масштаба и основных параметров.
2. Добавлено создание бота в мире через UI и серверный обработчик, чтобы бот появлялся как управляемый объект мира.
3. Исправлено сохранение имени бота: новое имя теперь применяется к выбранному боту и не теряется после сохранения.
4. Исправлена работа аватара бота: URL/путь аватара применяется, поддержана авто-загрузка avatar URL, исправлены сценарии, где модель не обновлялась после изменения.
5. Исправлено перемещение бота gizmo/body-drag в 3D-сцене, чтобы редактор корректно работал не только через числовые поля.
6. Добавлены AI-настройки бота: системный промпт, поведение ассистента, переменные шаблонов, fallback-сообщение и поддержка персонального API-ключа на уровне бота.
7. Добавлена поддержка Claude-моделей в настройках AI.
8. Добавлены настройки поведения: радиусы приветствия, прощания и слышимости чата, стационарный режим, авто-взгляд на ближайшего пользователя и пауза бота.
9. Добавлены игровые use-actions по клавише `E`: бот может реагировать на действие игрока рядом с ним.
10. Добавлены настройки анимаций и жестов: приветствие при приближении, idle-жест, реактивный жест на чат/жест игрока, дополнительные gesture slots, флаги жестов и тестирование жеста.
11. Добавлены настройки звука бота: audio URL, громкость, радиус слышимости, зацикливание и пространственный 3D-звук.
12. Исправлено воспроизведение bot audio в клиенте.

## Снаряжение и avatar preview

1. Добавлена серверная инфраструктура Gear и интеграция со screenshot-bot pipeline.
2. Добавлен тип объекта `ObjectType_GearItem` для корректной сериализации/обработки gear item.
3. Gear Inventory перенесён в расширяемый левый Qt editor dock с карточками экипированных и доступных предметов.
4. Новый `AvatarGearPreviewWidget` наследует `AvatarPreviewWidget`, имеет собственные OpenGL context и engine и использует тот же вид, камеру, управление и grounding, что preview в окне Avatar Settings.
5. Preview показывает avatar + animated bone attachments независимо от мировой сцены: он не использует world renderer, не переключает third-person и не меняет игровую камеру.
6. Добавлены orbit/pan/zoom, bone attachment, Move/Rotate/Scale по осям, точные transform-поля и Lucide icons; avatar/gear resources загружаются через существующий optimized/Basis-aware pipeline.
7. Восстановлены server handlers `QueryUserGear`, `CreateGearItem` и `GearItemUpdate`, authoritative обработка equipped gear в `AvatarFullUpdate`/`CreateAvatar`, owner/transform validation и persistence `gear_ids`/`equipped_gear_ids`.
8. Добавлен server capability `GEAR_INVENTORY_SUPPORT`: клиент не отправляет gear packets серверу без полного набора handlers и сохраняет соединение с несовместимым сервером.
9. Login, logout, disconnect, применение новой модели аватара и переход между мирами синхронизируют avatar settings и gear; сервер возвращает отправителю authoritative `AvatarFullUpdate`, чтобы клиент принял отфильтрованное состояние.
10. Локальные Windows-сборки `gui_client` и `server` прошли compile/link. По требованию владельца GUI не запускался; Linux production deploy и live create/equip/edit/reconnect требуют отдельной проверки и отдельного разрешения.

## Аудио-плеер

1. Добавлены radio streams и настройки activation distance для audio player.
2. Исправлен seek по реальному byte-range playback.
3. Доведены материалы и body styling аудио-плеера, сохранены черные controls.
4. Исправлены body texture и texture scale в webview.
5. Исправлена синхронизация emission/material и webview texture.
6. Исправлен потенциальный null dereference в `MP3AudioStreamer`, когда задан только `sound_file_path`.

## Карты, порталы и интерфейс

1. Добавлен OSM map world layer.
2. Добавлена priority download queue и dock UI для карты.
3. Доделан portal editor: материалы портала и runtime mapping.
4. Улучшена русская локализация интерфейса и layout world settings.
5. Добавлено меню Themes с пакетом Qt themes и синхронизацией Windows caption.
6. Добавлена кнопка-глобус в toolbar браузера для текущей web-mode локации.
7. Добавлено действие favorites star.

## XR, runtime и upstream

1. Исправлена синхронизация desktop companion mirror при активном XR.
2. Добавлены script timers и доработки запуска XR.
3. Интегрированы upstream-исправления Substrata 1.7.1-1.7.3 за апрель 2026.

## Технические детали релиза

- Версия клиента обновлена до `0.0.21` в `shared/Version.h` и `shared/MetasiberiaVersion.h`.
- Целевой Windows-артефакт релиза: `MetasiberiaBeta-Setup-v0.0.21.exe`.
- Релиз публикуется как GitHub `Pre-release`.
- Windows installer `.exe` не загружается агентом в GitHub Release; владелец релиза загружает его вручную после локальной проверки.
