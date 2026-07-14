# Native Voxel Editor

Назначение: каноническое описание встроенного C++ voxel editor Metasiberia, его совместимости с существующим `VoxelGroup`, инструментов, clipboard и процедурной генерации.

Проверено: 2026-07-14 по текущей реализации `master`; production не изменялся и не развёртывался.

## Что существовало до интеграции

- `Voxel` и `VoxelGroup` в `shared/WorldObject.h`: sparse-список только занятых координат с `mat_index`.
- 24-bit RGB-цвет через `WorldMaterial`; voxel хранит индекс материала, а не собственный RGBA.
- Zstd-сжатие, сеть, world persistence и XML через существующий compressed voxel blob.
- Greedy mesher в `shared/VoxelMeshBuilding.*`.
- Импорт MagicaVoxel `.vox` через общий model-loading path.
- Ctrl+ЛКМ добавлял один voxel, Alt+ЛКМ удалял один voxel.
- Общий `UndoBuffer` сохранял полные снимки `WorldObject`.

Не существовало специализированной панели, слоёв, palette metadata, delta undo, Brush/Eraser/Paint/Line/Box/Sphere/Fill/Picker/Select, clipboard, настраиваемых generators, Cubes render mode и voxel-specific hotkeys.

## Совместимое решение

Существующий wire/disk payload не заменён. `VoxelGroup` остаётся авторитетным sparse-форматом, поэтому старые клиенты и сервер продолжают читать геометрию. Новые editor metadata сохраняются в `WorldObject::content` с marker:

```text
metasiberia_voxel_editor_v1
```

В JSON сохраняются schema version, active layer, render mode, palette, recent colours, сохранённый legacy/user `WorldObject::content` и список слоёв с name/visible/locked/opacity/material indices/base opacities. `WorldObject::materials` остаётся таблицей фактических RGB-цветов. Для старого voxel-объекта обычное изменение physics/audio/material/content не запускает миграцию само по себе; при первом специализированном voxel metadata edit прежний content переносится в sidecar и продолжает отображаться в generic ObjectEditor.

Stage-1 layer — совместимая группировка непересекающихся material indices, а не отдельный voxel payload. `locked` блокирует геометрические и структурные операции только в native editor. `visible` и `opacity` задают множитель сохранённой базовой `WorldMaterial::opacity` (для hidden эффективное значение `0`), поэтому hide/show не превращает legacy alpha `0.35` в `1.0`. Voxels визуально скрываются, но пока остаются в общем mesh/physics payload. Два слоя не могут независимо хранить разные voxels в одной координате. Удаление слоя удаляет его voxels и сразу обновляет compressed payload; если это опустошило бы объект, остаётся один seed voxel в активном слое, потому что legacy renderer не загружает пустой `VoxelGroup`.

## Owners

| Область | Файлы |
| --- | --- |
| metadata/layers/palette | `gui_client/VoxelEditorData.*` |
| инструменты и delta command | `gui_client/VoxelTools.*` |
| процедурные generators/метрики | `gui_client/VoxelProceduralGenerator.*` |
| UID-isolated undo/redo | `gui_client/VoxelUndoStack.*` |
| Qt UI | `gui_client/VoxelEditorPanel.*` |
| menu/editor lifecycle/world input | `gui_client/MainWindow.*`, `gui_client/GUIClient.*`, `gui_client/UIInterface.h` |
| Cubes/Greedy meshing | `shared/VoxelMeshBuilding.*`, `gui_client/ModelLoading.*`, `gui_client/LoadModelTask.*` |
| Qt icon tinting | `gui_client/LucideIconUtils.*` |
| Lucide SVG/licence/provenance | `resources/icons/lucide/` |
| русская локализация | `gui_client/RuntimeTranslation.cpp` |

## Работа в клиенте

1. `Правка -> Добавить -> Добавить воксель` и верхняя toolbar-кнопка используют один существующий `QAction`.
2. Клиент создаёт один seed voxel и metadata schema 1, отправляет обычный `CreateObject`.
3. После подходящего ответа `State_JustCreated` существующий lifecycle выбирает объект, если он создан текущим пользователем менее 30 секунд назад и не имеет `SUMMONED_FLAG`; `State_InitialSend` сам по себе объект не выбирает.
4. `MainWindow` распознаёт `ObjectType_VoxelGroup`, показывает Voxel Editor наверху editor dock и оставляет generic transform controls ниже.
5. Включённый scene-edit mode применяет обычный ЛКМ только к physics object выбранного voxel object. Старые Ctrl/Alt операции сохранены.

Brush/Eraser/Paint применяют bounded stamp размером 1–16 с Cube/Sphere shape, Add/Replace/Paint mode, hollow и mirror XYZ. Line, Box и Sphere задаются двумя кликами. Fill выполняет атомарную 6-связную заливку только существующего компонента одного материала активного слоя: пустое бесконечное пространство не обходится, превышение cap не оставляет частичную команду. Picker выбирает существующий material. Select задаёт box двумя кликами, после чего доступны Copy/Paste/Delete/Duplicate/Move с независимым XYZ offset. Clipboard хранит относительные sparse voxels; Move является одной delta-командой и корректно обрабатывает overlap, collision mode и voxels другого слоя.

Каждая геометрическая операция, clipboard edit и procedural generation создаёт один `VoxelEditCommand` со списком `before/after` только для изменённых координат. `VoxelUndoStack` хранит неограниченные в коде undo/redo vectors отдельно по UID; практический предел остаётся памятью процесса. Перед непосредственным undo/redo клиент сравнивает текущую compressed/content/material revision с revision, на которой построена delta-история, и очищает stale history до мутации. Redo временно применяет команду, повторно проверяет итоговый parcel AABB и при отказе тем же stack transition восстанавливает и voxels, и redo entry до mesh/network update. Metadata, generic properties и transform по-прежнему используют общий full-object `UndoBuffer`. Чтобы две независимые истории не нарушали хронологию, metadata/generic edit является barrier и очищает voxel delta branch; это безопасно, но геометрическая история до barrier пока теряется. После одной принятой операции выполняется одна полная mesh/physics rebuild существующим path.

Горячие клавиши в активном Voxel Editor: `B` Brush, `E` Eraser, `P` Paint, `L` Line, `F` Fill, `I` Picker, `S` Select, `[`/`]` размер кисти; `Ctrl+Z`/`Ctrl+Y` сначала пробуют voxel delta history. Clipboard actions выполнены явными кнопками панели, поэтому глобальные `Ctrl+C/V` и `Delete` пока не перехватываются.

## Процедурная генерация

`VoxelProceduralGenerator` — независимый clean-room backend. Он строит детерминированную sparse delta-команду для `Box`, `Ellipsoid`, `Rock`, `Terrain Patch`, `Noise Volume`, `Crystal` и `Wall/Ruins`. Ни код, ни noise/tables Goxel не использованы.

Панель задаёт `seed`, origin XYZ, независимые size X/Y/Z (то есть параллелепипед больше не фиксирован около 8×8×8), wall thickness, hollow, clear-active-layer/merge, noise scale, threshold, density, octaves, detail и safety limit. Рядом рассчитываются footprint area, footprint perimeter, bounding volume и bounding surface area. Размеры UI ограничены 128 по оси; backend дополнительно нормализует до 256 и не разрешает одной операции более 1 000 000 generated/changed voxels. При выходе за cap generator атомарно отклоняет всю операцию и не сохраняет частичную фигуру. `clear active layer` удаляет только voxels активного material-index layer и генерирует замену в той же undo-команде; другие слои сохраняются. Несовместимая комбинация `clear active layer + Paint` отклоняется до удаления данных.

## Rendering

- `Greedy` — существующий оптимизированный mesher.
- `Cubes` — новый sparse surface mesher: отдельная quad для каждой открытой voxel face, внутренняя грань соседних voxels удаляется.
- `Marching Cubes` показан как disabled TODO; metadata сохраняется, voxel-data не преобразуется.

Render mode и transparency mask входят в async mesh cache key и передаются через `LoadModelTask`, поэтому Cubes/Greedy и разные visible/transparent material boundaries не используют устаревший cached mesh. Cubes output сортируется по координатам для детерминированного порядка геометрии.

## Goxel как reference и лицензия

Goxel изучен как архитектурный/функциональный reference: sparse 16³ blocks, разделение gesture/shape/mode, layers, palette, selection, exporter registry и render-only smooth mode. Решения Metasiberia `Greedy`, delta-command undo и поля layer `locked/opacity` не приписываются Goxel: в Goxel нет greedy mesher, undo использует image snapshots с copy-on-write blocks, а явных layer fields `locked/opacity` нет. Goxel распространяется по GPL-3.0-or-later; MIT совместима с GPL, но прямое включение Goxel-кода в распространяемый combined/derivative client потребовало бы GPLv3-compatible лицензирования всей соответствующей работы и выполнения source/notice obligations либо отдельного разрешения/коммерческой лицензии автора. Исходники, lookup tables, exporters, assets, icons и palettes Goxel не копировались; этот код написан независимо.

## Lucide icons

Для `Правка -> Добавить` каждому действию назначен семантический Lucide SVG: model/image, hypercard, text, spotlight, particles, voxel, tree, scientific object, camera, seat, audio, web, video, decal, portal и bot. Тот же набор используется Qt toolbar, а Voxel Editor получил иконки tools, layers, selection/clipboard, rebuild, generators и disabled import/export placeholders. `LucideIconUtils` рендерит SVG как alpha mask и tint-ит его выбранным Qt-цветом, поскольку Qt 5 не гарантирует theme resolution для SVG `currentColor`.

Использован ограниченный SVG subset из `lucide-icons/lucide` commit `b442632ee6fe6250bf24fef026e44244a33812c9` (2026-07-11). Полный upstream ISC notice и MIT notice для Feather-derived icons находятся в `resources/icons/lucide/LICENSE.txt`, provenance — в соседнем `README.md`. Код/ресурсы Goxel к этим иконкам отношения не имеют.

## Проверка

```powershell
gui_client.exe --voxel_editor_smoke C:\programming\voxel_editor_smoke.json
```

Narrow smoke проверяет metadata JSON round-trip/limits и escaping legacy content; Brush/Paint/Mirror/locked layer; Line; bounded/atomic Fill; copy/delete/paste/duplicate/move и collision rollback; прямые `VoxelEditCommand::undo/redo`; per-UID stack push/isolation/clear; все семь procedural generators, determinism, hollow, clear/merge, safety cap, независимые размеры и метрики; QWidget round-trip; сохранение удаления слоя в compressed payload; legacy content migration; разные base alpha после hide/show; shortcuts `B/L/F/S`/`]`; runtime Lucide icons; ожидаемые 12 Greedy против 20 Cubes triangles для двух соседних voxels. Он не вызывает live `VoxelUndoStack::undo/redo` или `Ctrl+Z`/`Ctrl+Y` через `MainWindow` и не проверяет реальный menu/toolbar → `CreateObject` → server selection lifecycle, live mouse ergonomics, визуальный render/physics, reconnect, network persistence или production.

## Ограничения и следующие этапы

- Нет chunk storage/dirty chunk rebuild: mesh/physics пока пересобираются целиком.
- Нет независимых перекрывающихся layer payloads и layer blend modes.
- Нет continuous interpolated drag stroke и визуального wireframe/handles выделенной области; bounds подтверждаются уведомлением и используются кнопками панели.
- Нет Marching Cubes, smooth normals или threshold renderer.
- `.vox` import существует в общем loader, но native panel import/export, OBJ/PLY/PNG/Qubicle exporters ещё не реализованы.
- Ray tracing и unlimited world coordinate range не реализованы; generators имеют намеренные dimension/operation caps.
- Delta history и общий full-object `UndoBuffer` пока не объединены; chronology barrier предотвращает неверный порядок ценой очистки более старой delta-ветки.
- Expected revision хранится как один последний hash на UID. Запоздалый server echo предыдущей из нескольких быстрых локальных voxel-команд может консервативно очистить history; это не применяет stale delta и не повреждает объект, но теряет undo-ветку. Полное устранение требует очереди подтверждаемых revisions/operation IDs.
- Qt panel является native-client feature; SDL/Web получает default no-op bridge и сохраняет legacy Ctrl/Alt behavior.

Следующая безопасная граница: ввести отдельный versioned compressed layer payload с backward-compatible flattening, затем chunk dirty tracking и selection overlay/manipulator. Любое изменение shared/wire persistence требует совместного client/server migration plan.
