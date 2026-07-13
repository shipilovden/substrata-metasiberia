# Текущее состояние Metasiberia

Назначение: разделять committed baseline, active working tree, partial features, планы и неизвестное.

Снимок: 2026-07-11. `master` / `2ef62fd6`, синхронно с `origin/master`; version headers `0.0.21`. Tag `v0.0.21` указывает на более ранний `f010eeb2`, поэтому HEAD содержит unreleased изменения.

Рабочее дерево dirty: пользовательские изменения client/particle/scientific code, документация Phase 1/2 и новые `AGENTS.md`. Ничего из этого не является committed baseline, но Scientific Object Editor по указанию владельца классифицируется как официальный WIP.

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
- Optional CEF/browser path существует, но canonical local Qt wrapper обычно CEF отключает.

## Официальный WIP: Scientific Object Editor

Подтверждено в текущем dirty tree:

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

## Другие активные изменения рабочего дерева

- Расширение professional Particle Editor после committed `2ef62fd6`: audio, colour/trail/runtime controls и связанные client changes.
- Procedural Tree Editor foundation/follow-up: Qt Add Tree action translated as "Добавить дерево", normal ObjectEditor transform block above `TreeEditorPanel`, `TreeParams`/presets/serialization, seed-driven C++ generator for trunk/branches/billboard leaves, generic `WorldObject` marker `metasiberia_tree_object_v1`, generated `.bmesh` derivative, imported EZ-Tree leaf/bark assets under `resources/tree_assets`, and expanded EZ-Tree-style saved params. Narrow `RelWithDebInfo gui_client` build and `--tree_generator_smoke` passed 2026-07-14; full manual server-confirm/reconnect/second-client flow not yet verified.
- Documentation/Knowledge System Phase 2/3/5: `docs/codex`, MCOS, AGENTS, glossary и debt register.

Windows/Qt `gui_client` canonical wrapper build подтверждён 2026-07-11 11:41 local time для Release + RelWithDebInfo, XR Auto ON. После follow-up overlay/image/localisation fix узкий `RelWithDebInfo gui_client` target и PubChem apply/molecule-information smokes прошли из RelWithDebInfo runtime; full manual GUI/server/reconnect flow, server runtime, deploy and production не выполнялись.

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
