# Metasiberia Beta v0.0.22

Дата: 2026-08-24

## Главное

Metasiberia Beta v0.0.22 — крупный релиз инструментов создания контента. В нём появились полноценная поддержка 3D Gaussian Splatting, редактирование ландшафта, процедурные деревья, частицы, воксели, научные и культурные объекты, а также новые панели для анимаций, документов, фото и видео.

Одновременно расширены Bot Editor и система снаряжения аватаров, переработаны карты, чат и основные панели интерфейса, улучшены WebClient, XR и Windows runtime.

## 3D Gaussian Splatting

1. Gaussian splats стали полноценными объектами мира, которые загружаются и отображаются непосредственно в основной 3D-сцене.
2. Добавлен нативный OpenGL renderer с сортировкой splat-элементов относительно камеры и обычным source-over alpha blending.
3. Добавлен отдельный редактор GaussianSplats с пресетами Original, Clean Haze и Metasiberia Clear.
4. В редакторе доступны фильтрация слабых исходных splat-элементов, плотность, яркость, радиус, насыщенность, контраст, edge alpha и ограничение степени spherical harmonics.
5. Поддерживается прямое чтение стандартных 3DGS PLY, SPLAT и SPZ v4.
6. Через встроенный self-hosted converter поддерживаются compressed PLY, KSPLAT, SPZ v2–v3, SOG, LCC и LCC2.
7. Gaussian-ресурсы проходят через стандартную систему загрузки, хранения и проверки ресурсов Metasiberia.
8. Загруженный splat можно выделять, перемещать, вращать и масштабировать стандартными инструментами трансформации.
9. Добавлены специализированные shaders, converter/viewer resources и лицензии используемых компонентов.

## Terrain и ландшафт

1. Добавлена точная работа с EXR heightmap для digital-twin terrain.
2. Добавлены независимые reference masks для дорог и зданий с отдельным включением и отключением.
3. В World Settings появилась нативная вкладка «Скульптинг».
4. Реализованы четыре базовых инструмента редактирования terrain, настройка размера и силы кисти, Undo и Redo.
5. Добавлен режим свободного вращения камеры правой кнопкой мыши во время работы с ландшафтом.
6. В 3D-сцене отображаются внешняя граница кисти и зона falloff.
7. После завершения штриха перестраиваются terrain geometry и связанная растительность.
8. Изменённая heightmap автоматически сохраняется обратно в EXR.
9. Настройки мира, карты, освещения, тумана и terrain сохранены в обновлённой tabbed-компоновке World Settings.

## Редакторы и инструменты

1. **Tree Editor:** добавлен нативный процедурный workflow на основе EZ-Tree с 16 импортированными пресетами, upright Z-up geometry, leaf assets и миграцией ранее созданных деревьев.
2. **Particle Editor:** добавлены emitter shapes, presets, библиотека встроенных и пользовательских sprites, curves, quick recipes, preview bursts, glow/soft rendering, forces, attractor, vortex, black-hole behaviour, collisions и diagnostics.
3. Для частиц добавлены Lua-команды `emitter.start()`, `emitter.stop()`, `emitter.burst()` и `emitter.clearParticles()`.
4. **Voxel Editor:** добавлены Line, Fill и Selection, clipboard-операции, procedural generators, метрики размеров, bounded delta Undo/Redo и режимы рендеринга Cubes и Greedy.
5. Voxel-редактор ограничивает изменения границами доступного parcel и атомарно отменяет недопустимые сложные операции.
6. **Scientific Object Editor:** добавлены интеграция PubChem, русские поисковые запросы, выбор атомов и связей, подписи, измерения расстояний, углов и torsion, изображения и информационные карточки.
7. Добавлена встроенная периодическая таблица всех 118 элементов с карточками, поиском, несколькими представлениями и подсветкой элементов текущей молекулы.
8. **Cultural Object Editor:** добавлены каталоги Art Institute of Chicago и The Met, поисковые фильтры, IIIF previews, метаданные источника и сведения о правах.
9. Публичные изображения произведений можно сохранять как world resources и размещать в мире на одностороннем выставочном Quad.
10. **Animation Editor:** добавлены библиотека и поиск анимаций, preview и playback, loop, speed, blending, назначения idle/walk/run/jump, transitions, root motion, mirroring, events и таблицы skeleton retargeting.
11. Добавлены локальные animation profiles, Undo/Redo и интерфейс импорта GLB, glTF, FBX, BVH и VRM. Полное применение профилей к runtime animation graph остаётся отдельным этапом.
12. **Document Editor:** добавлена нативная панель для TXT, HTML и Markdown с редактированием, preview, поиском, сохранением и экспортом bounded descriptor.
13. **Photo/Video Settings:** добавлена панель параметров камеры, видео и output, presets и настройки оптики и кадра, интегрированные с существующими screenshot/gallery flows. Backend видеозаписи и кодирования в этом релизе не заявляется как завершённый.
14. В Add-меню добавлены новые базовые формы: cone, pyramid, wedge, octahedron, triangular prism и hexagonal prism.
15. Общий Transform Gizmo интегрирован в специализированные редакторы и редактирование обычных объектов.

## Bot Editor и AI

1. Bot Editor расширен режимами Stationary, Patrol и Wander, маршрутными точками, скоростью перемещения и радиусом свободного движения.
2. Добавлены отдельные анимации приветствия, прощания, ходьбы, разговора и взаимодействия.
3. Расширены настройки пространственного звука: минимальная дистанция, задержка запуска и отдельные URL для приветствия, прощания и interaction.
4. Добавлены несколько use-actions и фильтрация игроков по UUID.
5. AI-настройки дополнены выбором provider, `top_p`, `top_k`, penalties, ограничением контекста и визуальным dialog tree.
6. Добавлены scripted responses, whitelist, blacklist, tool functions, webhook, active hours, timeout диалога и почасовые ограничения LLM.
7. Добавлены память игроков, reputation и quest-поля, content filter и jailbreak guard.
8. Реализованы per-player rate limits, кэш ответов, fallback provider и повторные запросы при ошибке.
9. Администратор получил conversation log, player CRM и возможность отправить сообщение вручную.
10. Добавлены duplicate, templates, preview системного prompt и полный JSON export/import настроек бота.
11. Добавлена подготовительная MCP-compatible интеграция с обработкой `render_view` и HTTPS forwarding. Listener остаётся выключенным по умолчанию и не объявляется публичным сетевым сервисом.

## Avatar/Gear

1. Gear Inventory получил полноценный realtime-редактор экипировки.
2. Добавлены немедленное изменение имени и transform, выбор Move/Rotate/Scale, управление по осям и локальный preview без ожидания server echo.
3. Снаряжение редактируется в изолированной OpenGL-сцене avatar preview и не изменяет мировую камеру или основной renderer.
4. Добавлены карточки предметов, thumbnails, hover-подсказки, подсветка выбранного предмета и удаление.
5. Gizmo привязан непосредственно к выбранному предмету экипировки и использует стандартные стрелки и дуги вращения редактора мира.
6. Удаление и синхронизация Gear выполняются авторитетно сервером с проверкой владельца и сохранением состояния.
7. Создание нового аватара перенаправлено в специализированный сервис `avatars.metasiberia.com`; устаревшая вкладка VRoid удалена.

## Карты и UI

1. Добавлена потоковая загрузка OSM tiles для map world и серверный endpoint `/osm_tile`.
2. На встроенной карте отображаются пользователи и боты, hover-имена и направление локального игрока относительно камеры.
3. Map dock доступен во всех мирах Metasiberia, а переходы между мирами одного сервера используют sub-URL flow.
4. Чат переработан в tabbed-интерфейс с компактным Telegram-подобным полем ввода.
5. Добавлены private dialogs, локальные группы, unread indicators, reactions, context menu, локальное удаление, attachments, audio previews и chat sounds.
6. Реализована серверная маршрутизация приватных сообщений по UID и загрузка вложений через resource URLs.
7. Основные Add-действия, меню и editor controls получили единый набор Lucide SVG icons.
8. Иконки поддерживают high-DPI и автоматически адаптируют цвет к выбранной светлой или тёмной теме.
9. Обновлены toolbar, menu chrome, темы, русские переводы и поведение editor docks.
10. Завершена миграция пользовательской идентичности приложения с Substrata на Metasiberia.

## WebClient

1. В Emscripten/WebClient добавлена отдельная панель загрузки Gaussian splats.
2. Поддерживаются выбор отдельного файла и выбор sidecar-папки для составных SOG/LCC assets.
3. Browser-side converter приводит поддерживаемые форматы к canonical binary PLY и передаёт результат в стандартный поток создания объекта мира.
4. В панели отображаются этапы чтения, конвертации и создания объекта, а также сообщения об ошибках.
5. Созданный Gaussian Splat можно выделить и редактировать стандартными transform controls WebClient.
6. Converter и viewer поставляются как self-hosted resources и не требуют стороннего облачного сервиса конвертации.

## Исправления ошибок

1. Во внешней зависимости `glare-core` исправлено выделение WebGL framebuffer для первого post-processing downsize: размер начальных данных теперь соответствует округлённым размерам texture при нечётной ширине или высоте viewport.
2. В WebClient восстановлена загрузка пользовательских аватаров `.bmesh`, `.glb`, `.gltf` и `.vrm`, когда сервер объявляет optimized meshes, но подходящий derivative или постоянный source cache отсутствует.
3. Исправлен parcel shader при отключённом Order Independent Transparency: сетка и границы участков снова выводятся через обычный colour target.
4. Qt-only Cultural Object Editor и обработка изображений изолированы от общей SDL/Emscripten сборки, устраняя зависимость WebClient от Qt headers и Cultural API classes.
5. Исправлено чтение допустимых бесконечных opacity-значений в 3DGS PLY; обычные `.ply` больше не отправляются ошибочно в CEF converter fallback.
6. Исправлена синхронизация terrain brush и точки редактирования: штрих применяется точно под отображаемой проекцией курсора.
7. Исправлены зеркальная Y-координата sculpt stamp, рассинхронизация с асинхронной перестройкой terrain и артефакты устаревшей geometry.
8. Исправлена потеря choices при переключении узлов dialog tree Bot Editor.
9. JSON export/import Bot Editor теперь сохраняет полный набор avatar, animation, audio, movement, AI, dialog, memory, safety и fallback-настроек.
10. Avatar и audio resources бота загружаются до отправки обновления серверу.
11. Исправлены выбор процедурных деревьев, Z-up ориентация, применение EZ-Tree presets и восстановление legacy placeholder trees.
12. Исправлены исчезающие Gear thumbnails и gizmo, а также положение gizmo относительно экипированного предмета.
13. Исправлено отображение alpha у встроенных и пользовательских particle sprites.
14. Исправлены native zoom, fallback tiles, tile cache, refresh, reconnect и восстановление service world для карт.
15. Стабилизирована screenshot-bot инфраструктура: listener сохраняется при reconnect, GUI slave переиспользуется, временные файлы очищаются после сохранения.
16. Object Editor и Bot Editor больше не перекрываются одновременно в одном dock.

## XR, runtime и upstream

1. OpenXR swapchain textures теперь подключаются непосредственно к framebuffer без зависимости от отсутствующего локального расширения `glare-core`.
2. XR swapchain path проверен на Quest 2 через ALVR/SteamVR с активной stereo-сессией и pose tracking.
3. Добавлена поддержка визуализации контроллеров Quest и связанных capture-сценариев.
4. `openxr_loader.dll` включается в Windows runtime package для XR-enabled сборки из чистого output-каталога.
5. CEF включён в канонический Windows Qt workflow; добавлена проверка link artifacts и обязательного runtime-набора.
6. Документированы раздельные Qt 5 и Qt 6 workflows; релизная ветка `master` продолжает собираться каноническим Qt 5 pipeline.

## Технические детали релиза

- Версия клиента обновлена до `0.0.22` в `shared/Version.h` и `shared/MetasiberiaVersion.h`.
- Source state, из которого собран installer до добавления этого файла release notes: `62d4fae63ddbdba2eec15866f74e5c211fdd86a9`.
- Внешняя зависимость `glare-core`: `d4586dc78451f28f4ec773126f707ca0bd3099c3`.
- Платформа: Windows x64.
- Toolchain: Qt `5.15.16`, Visual Studio 2022, CEF и OpenXR.
- Windows installer создан локально существующим Qt Release pipeline и NSIS installer pipeline.
- Для упаковки использован `scripts/create_simple_installer.ps1`: скрипт принимает подготовленный полный Qt Release runtime, создаёт NSIS package и вызывает установленный `makensis.exe`.
- `scripts/publish_update.ps1` и автоматический release pipeline не использовались.
- Installer: `MetasiberiaBeta-Setup-v0.0.22.exe`.
- Размер installer: `409 914 094` байта.
- SHA-256 installer: `4EAD62F7C97214139169998CFC3FF1E8D3BEE94A93841A0356FDF69DBFFB5843`.
- Installer не запускался; installation smoke не выполнялся.
- Installer хранится локально и не прикладывается к GitHub Release.
- В GitHub Release остаются только стандартные assets `Source code (zip)` и `Source code (tar.gz)`.
