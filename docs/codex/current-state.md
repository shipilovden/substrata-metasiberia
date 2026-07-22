# Текущее состояние Metasiberia

Назначение: разделять committed baseline, active working tree, partial features, планы и неизвестное.

Снимок: 2026-07-22. Перед текущим CEF workflow patch проверен `master` / `78375f52`; version headers остаются `0.0.21`. В текущий master-набор входят native editors, MCP integration и документация; production deploy не выполнялся.

Scientific Object Editor, Particle Editor, Procedural Tree Editor, Native Voxel Editor и Lucide UI integration входят в committed baseline `master`; production/runtime readiness по-прежнему определяется отдельными evidence ниже, а не фактом commit.

## Active working tree 2026-07-17: Gear Inventory

- Qt Gear Inventory размещён в общем левом editor dock. `AvatarGearPreviewWidget` наследует `AvatarPreviewWidget`, имеет собственные OpenGL context/engine/scene и повторяет вид, камеру, orbit/pan/zoom и grounding окна Avatar Settings; avatar + animated bone attachments строятся внутри preview и не используют мировой renderer.
- Preview resource path использует текущие server-provided avatar/gear state, optimized/Basis fallbacks и async `ResourceManager`. Он не переключает third-person, не меняет world camera и не рисует основную сцену повторно.
- Client sync защищён login/connection/`GEAR_INVENTORY_SUPPORT` guards, проверкой inventory UID и наличия собственного avatar. Server authoritative path валидирует ownership, transforms, duplicates/limit, сохраняет `gear_ids`/`equipped_gear_ids`, восстанавливает gear при login/world transition и возвращает authoritative `AvatarFullUpdate` с настройками аватара и экипировкой.
- Локальные Windows-сборки `gui_client` и `server` compile/link прошли. По требованию владельца GUI не запускался; live create/equip/edit/reconnect, Linux production deploy и screenshot-bot flow не проверялись. Production не менялся и требует отдельного разрешения.

Каноническая детализация: [inventory-system.md](inventory-system.md).

## Реализовано в committed baseline

- Native Qt client и alternative SDL path в target `gui_client`.
- Emscripten/Web Client path общей C++ codebase и HTML shell.
- Realtime C++ server с TLS TCP, UDP, worlds/users/resources/scripting и custom persistence.
- Embedded HTTP/HTTPS/WebSocket server, Server Website, account/auth routes, Admin `/admin*`, resources/photos/screenshots/map delivery.
- Shared protocol version 62, включая private chat message 2001 и существующие bot/gear contracts.
- Object/material/parcel/world/bot/gear/shader/particle editor surfaces.
- Screenshot bot + GUI screenshot slave mode.
- Real-map world/tile pipeline и protected map-scale behavior.
- Manual Windows release policy и configure-smoke CI.

## Реализовано частично

- Chat redesign: private messages и часть UI/attachments существуют; groups, delivery/history/reactions/voice и другие plan stages неполны.
- Bot editor: extensive UI/data path есть, field/runtime coverage требует отдельной проверки.
- Web Client: source/build path есть; current reproducible production deployment не подтверждён.
- Qt 6: compatibility code присутствует, canonical build остаётся Qt 5.
- Wiki: первые onboarding pages существуют, roadmap значительно шире реализации.
- CEF/browser path compile-time optional, но canonical Windows Qt wrapper теперь включает CEF по умолчанию. Release + RelWithDebInfo compile/link/runtime-copy с CEF 139.0.40 подтверждены 2026-07-22; фактический 3D WebView/browser UI не запускался и остаётся manual verification.

## Официальный WIP: Scientific Object Editor

Подтверждено в текущей реализации:

- Qt editor/settings files зарегистрированы в client CMake/MOC;
- Add Scientific Object action и переключение editor lifecycle в `MainWindow`;
- generic `WorldObject` с marker/JSON в `content`;
- metadata/source/provenance/status/data/visualisation/physics/measurement/animation/simulation/AI/custom fields;
- source status path with `Idle/Ready/Unsupported/Error`;
- explicit built-in molecule samples `Caffeine` and `Water`, marked as local sample data rather than external provider data;
- PubChem Phase 1 molecule path: interactive PUG REST search, explicit CID selection, property JSON, SDF structure parse, PNG preview/cache on CID application, throttling/backoff, provenance/cache/checksum fields;
- PubChem HTTPS runtime fix: local QtNetwork build has SSL disabled, so PubChem uses Windows WinHTTP/SChannel; headless client network smoke confirmed `water` CID 962 and `nicotine` CID 89594 over HTTPS with SDF 3D and PNG responses;
- PubChem selected-result application fix: QWidget apply smoke confirmed visible selected CID, one-click `Загрузить CID ... в объект`, source transition `manual` -> `PubChem`, PNG cache assignment and local molecule OBJ generation for `water` and `nicotine`;
- Phase 1.2 molecule information layer: selected-object atom/bond ray picking, native molecule viewport, selected-object 3D world-space label/highlight overlays, five selection states, atom/bond/molecule context menus, seven label modes, CPK legend, saved distance/angle/torsion and derived molecule metrics;
- molecule card with immediate Summary/Structure/REST properties and lazy cached PUG View description/properties/classification/safety/bioactivity/drug/literature/related records/patents/spectra sections;
- lazy PubChem image viewer with fit/zoom/wheel/pan/reset/open/save and source/license display;
- exact-first Russian query resolver confirmed for `вода`, `никотин`, `аспирин`, `кофеин`, `этанол`, `глюкоза`;
- native periodic table module with 118 elements, table/list/3D property graph and molecule-element selection integration;
- provider/computed/user classification fields, favorites, recent CIDs and search history; unsupported simulation UI is disabled and Rotation/Spin is limited to the native molecule viewport;
- prompt code templates, molecule atom/bond parsing, temporary OBJ/MTL generation и generic content-addressed bmesh conversion path;
- payload size guard before writing `WorldObject::content`;
- unknown root JSON field preservation;
- non-solid/non-collidable default Scientific Object physics, with existing WorldObject collidable/dynamic/sensor/mass/friction/restitution controls;
- Windows/Qt `gui_client` build confirmed 2026-07-11 via `C:\programming\qt_build.ps1` for `Release` and `RelWithDebInfo`;
- narrow PubChem network smoke confirmed 2026-07-11 via `gui_client.exe --scientific_pubchem_smoke <report.json>`;
- narrow PubChem object-application smoke confirmed 2026-07-11 via `gui_client.exe --scientific_pubchem_apply_smoke <report.json>`;
- narrow information-layer smoke confirmed 2026-07-11 via `gui_client.exe --scientific_molecule_info_smoke <report.json>`: six Russian queries, seven labels, five selection states, three measurements, lazy classification and 118 periodic records;
- existing generic CreateObject/ObjectFullUpdate и persistence reuse.

Не подтверждено как реализованное:

- online database adapters кроме PubChem molecule Phase 1;
- внешние AI calls и исполнение generated code;
- общий import/parser для перечисленных file formats;
- persistent/cross-client labels through `ObjectType_Text` child-object lifecycle and SDL/Web parity for native information overlays; selected-object Qt/GL world-space overlays are implemented, but they are editor-local, not server entities;
- trajectory/vibration/time-series animation, simulation/MD/CFD/orbital solvers, area/volume solvers and global indexed catalog search;
- отдельный scientific shared/server type, API, database или migration;
- full manual GUI flow with server confirmation, generated-model `.bmesh` upload/reload/reconnect and SDL/Web/XR parity;
- runtime verification of payload guard and generated-model fingerprint behavior;
- production-ready credential security.

Каноническая детализация: [scientific-object-editor.md](scientific-object-editor.md).

## Редакторская интеграция текущего master

- Native Voxel Editor: существующий `VoxelGroup` и network/disk blob сохранены; добавлены marker metadata с legacy-content sidecar/base material opacity, material-index layers, 24-bit RGB palette/recent colours, Brush/Eraser/Paint/Line/Box/Sphere/Fill/Picker/Select, bounded mirror/hollow stamps, clipboard Copy/Paste/Delete/Duplicate/Move, per-UID delta undo/redo, Greedy/Cubes rendering, transparency-aware async cache, Qt panel и русская runtime translation. Процедурный backend создаёт Box/Ellipsoid/Rock/Terrain/Noise/Crystal/Wall с независимыми XYZ dimensions, seed/noise/detail/wall controls, area/perimeter/volume/surface metrics и atomic clear/merge. `Правка -> Добавить` и voxel buttons используют лицензированный Lucide SVG subset. Слои пока не имеют независимых перекрывающихся payloads; hidden/opacity не исключают voxels из общего mesh/physics, а metadata/generic edits служат barrier и очищают отдельную delta-историю. Marching Cubes, chunk rebuild и panel import/export остаются TODO. Расширенный Release smoke прошёл 2026-07-14; canonical Release + RelWithDebInfo wrapper и final smokes фиксируются в [build-and-test.md](build-and-test.md). Live server/reconnect/production deploy не выполнялись. Каноническая детализация: [voxel-editor.md](voxel-editor.md).
- Расширение professional Particle Editor после committed `2ef62fd6`: audio, colour/trail/runtime controls и связанные client changes.
- Procedural Tree Editor follow-up: Add Tree использует default `ash_medium`, random seed и generic marker `metasiberia_tree_object_v1`; JSON schema 2 автоматически перестраивает старые schema-1 meshes только при наличии edit permission. C++ generator повторяет фактический EZ-Tree runtime: exact 16 presets, Marsaglia RNG, quaternion branch orientation, terminal leaders, parent-relative radii, stratified final-branch leaves и engine-space Z-up output без повторного OBJ rotation/scale. Leaf assets перекодированы lossless в RGBA8, поэтому alpha не теряется в Basis path; Ash использует `ash.png`, Aspen — `aspen.png`. Realtime aliases/ranges/engine axes согласованы с сайтом, stale migration timers не переживают смену объекта. `--tree_generator_smoke` и `--tree_editor_smoke` проходят в Release и RelWithDebInfo; canonical Qt wrapper (XR Auto ON) завершился success 2026-07-14. Отдельный Blender render подтвердил вертикальный trunk, natural crown и cutout leaves. Owner runtime до fix воспроизвёл invisible/lying trees; post-fix live-world/reconnect/second-client visual flow ещё не выполнялся.
- Documentation/Knowledge System Phase 2/3/5: `docs/codex`, MCOS, AGENTS, glossary и debt register.

Windows/Qt `gui_client` canonical wrapper build повторно подтверждён 2026-07-14 для Release + RelWithDebInfo, XR Auto ON. Tree generator/editor и voxel editor smokes прошли из обеих canonical конфигураций; Windows `server` target также компилируется и линкуется. Full manual GUI/server/reconnect flow, Linux server runtime, deploy and production не выполнялись.

Owner launch invariant зафиксирован 2026-07-15: пользовательский `RelWithDebInfo` runtime всегда берётся из `C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe`. `C:\programming\substrata_output` остаётся legacy output и не является evidence для UI-проверки.

## Экспериментально

- Native OpenXR: рабочий code path по project notes, option default `OFF`, SDK внешний.
- Qt6 migration branches/guards.
- C++ installer, CV/lightmapper/stress/backup bot projects вне root baseline.
- Ethereum/NFT и optional external integrations: current product readiness не подтверждена.
- Tracked Emscripten build-cache directories — historical/generated, не reproducible source.

## Запланировано

- Последующие chat subsystem stages.
- Cesium/geospatial integration plan.
- Camera/world streaming plan.
- Расширение Wiki и отдельных editor UX.
- Полная Qt6 migration и Android/Web packaging decisions.
- Scientific platform goals MCOS Appendix B — только нормативная целевая модель до code evidence.

## Устарело/историческое

- v2 deploy/snapshot paths и `/root/cyberspace_server_state` как current production instruction.
- Direct OpenStreetMap client URLs, host-specific Basis workaround и прежние XR visibility workarounds.
- `local_backups/` и `emscripten_build*` как source of truth.
- Protocol password reset 8010/8011, помеченный obsolete.
- Release/incident notes прошлых версий — historical record.

## Неизвестно / требует проверки

- Live production services, DNS, active state symlink и deployed assets на 2026-07-11.
- Source/deploy Public Website, TheRift и avatar service.
- Root-disabled auxiliary target buildability/ownership.
- Отсутствующий `features.htmlfrag` и staging `server_dist_resources`.
- Security properties auth/session/local credentials.
- End-to-end manual UI behavior Scientific Object PubChem search/load, server-confirm/reconnect flow and current dirty Particle changes.

## Условия обновления

- Feature меняет статус только по code + соответствующему build/runtime evidence.
- После commit Scientific WIP переносится из working-tree section в baseline с точным commit.
- Structural/protocol/persistence change обновляет project map, architecture, relations, data map и ADR.
- Runtime status всегда хранится как dated snapshot.
