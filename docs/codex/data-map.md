# Карта данных и конфигураций

Назначение: определить существующие data types, owners, formats, persistence, sensitivity и change boundaries.

Проверено: 2026-07-14 по models/serializers/runtime docs, Scientific Object WIP и Procedural Tree assets/resources. Secret values не читались и не приводятся; production paths — dated documentation, не live validation.

## Доменные типы в коде

| Тип/группа | Owner | Network/disk роль | Основные consumers | Риск изменения |
| --- | --- | --- | --- | --- |
| `UID`, `UserID`, `ParcelID` | `shared/` | stable identifiers | client/server/web/bots/state | высокий: identity/routing |
| `WorldObject` | `shared/WorldObject.*` | central network + disk model | editors/render/server/state/scripts | максимальный |
| `WorldMaterial` | `shared/WorldMaterial.*` | object material serialization | client/render/server/resources | высокий |
| `WorldSettings`, `WorldDetails` | `shared/` | world configuration/state | client/server/web | высокий |
| `Parcel` | `shared/Parcel.*` | ownership/geometry/permissions | client/server/web/admin | высокий |
| `Avatar` | `shared/Avatar.*` | avatar state | client/server | высокий |
| `Resource`, `ResourceManager` metadata | `shared/` + runtime dirs | checksum identity and metadata | upload/download/server HTTP | высокий |
| `GearItem`, gesture settings | `shared/` | protocol/model data | client/server | medium/high |
| Users, sessions, bots, screenshots, photos, map metadata | `server/ServerWorldState.*` and related types | server-owned records | realtime/web/admin/persistence | высокий/чувствительный |
| Chat messages/history | shared protocol + client app data + server routing | wire + local history | client/server | высокий/privacy |
| Particle settings | `gui_client/ParticleEmitterSettings.*` | marker/serialized `WorldObject::content` | Qt/client render, generic server storage | WIP/client contract |
| Scientific settings | `gui_client/ScientificObjectSettings.*` | marker/JSON in `WorldObject::content` | Qt editor, generic server storage | WIP/high, 10 KB bound |
| Procedural tree settings | `gui_client/TreeParams.h`, `TreeSerialization.*` | marker `metasiberia_tree_object_v1`, current JSON schema 2 in `WorldObject::content` | Qt tree editor/generator, generic server storage | WIP; schema 1 mesh/parameter upgrade is client-side |
| Voxel editor metadata | `VoxelEditorData.*`; geometry remains shared `VoxelGroup` | marker `metasiberia_voxel_editor_v1` in content + existing compressed sparse payload/materials | Qt tools/layers/palette/clipboard/generators; generic server storage | high compatibility: metadata/client-only, wire payload unchanged |

Repo не содержит подтверждённой SQL schema/ORM для main server. `db_dirty` относится к custom record database.

## Version-controlled data

| Данные | Путь/формат | Producer/owner | Правило |
| --- | --- | --- | --- |
| Build config | CMake, `cmake/`, `scripts/config*.rb` | build system | менять с affected targets/CI |
| Protocol/models/versions | `shared/*.h/.cpp` | client/server developers | compatible/versioned change only |
| Runtime client assets | `resources/`, `shaders/` | asset pipeline | derivative; проверить copy/web/XR |
| Lucide UI subset | `resources/icons/lucide/*.svg`, `LICENSE.txt`, `README.md` | upstream Lucide commit pinned in provenance | ISC/MIT notices must ship with copied SVGs |
| Editable asset masters | `source_resources/`, `icons/` | artists/developers | сохранять originals/provenance |
| Server seed assets | `server_dist_resources/` | unknown staging owner | staging step требует подтверждения |
| Server Website assets | `webserver_public_files/`, `webserver_fragments/` | web developers | disk deploy отдельно от binary |
| Web Client shell | `webclient/webclient.html` | web/client developers | generated JS/WASM/data не source |
| Test fixtures/fuzz corpus | `testfiles/` | tests/fuzzers | task-only; проверить provenance/PII |
| Tracked generated caches | `emscripten_build*` | historical CMake | не редактировать/не считать portable |
| Local archives | `local_backups/` | developer recovery | non-canonical, potentially sensitive |

## Runtime/server data вне Git

| Данные | Location/format | Owner | Изменение/backup | Sensitivity |
| --- | --- | --- | --- | --- |
| Authoritative state | `<state>/server_state.bin`, custom binary records | `server` | app/migration only; full backup/restore smoke | critical |
| Server config | `<state>/substrata_server_config.xml` | operator/server parser | reviewed edit; backup | high |
| Credentials | `<state>/substrata_server_credentials.txt` | operator | secret provisioning/rotation only | critical |
| Uploaded resources | `<state>/server_resources/` | server/resource manager | immutable by checksum | high/user content |
| Deployed web copies | state public/fragments/webclient dirs | approved deployment | source remains Git; drift possible | low/operational |
| Photos/screenshots | runtime dirs | users/services | app-managed; backup/privacy | high |
| OSM tile cache | namespaced PNG hierarchy | embedded webserver | rebuildable, atomic writes | low/expensive |
| Map progress artifacts | CSV/log/JSON/session cache | maintenance services | service-managed | mixed; session secret |
| External TheRift data | external DB/backups per runbook | external service | schema/source outside repo | high |

Active production state path/symlink must be live-checked immediately before any state operation.

## Client-local data вне Git

| Данные | Формат/location | Правило | Sensitivity |
| --- | --- | --- | --- |
| QSettings/profile | OS-specific app config | user-owned; do not commit | high if credentials/tokens |
| Resource cache/database | app-data | regenerable; not source asset | low/possibly user content |
| Chat history | `chat/history_v1.json` | private; scrub before fixtures | high |
| Logs/traces | log, CEF log, `xr_pose_trace.csv` | share minimal fragments | medium/high |
| Screenshots/recordings | user directories | private media | high |
| OAuth/app credentials | env/local store/QSettings | never Git/log | critical |

## Scientific Object WIP

Каноническая детализация текущего WIP: [scientific-object-editor.md](scientific-object-editor.md). Каноническая справка по будущим external providers, formats, cache и provenance: [scientific-data-providers.md](scientific-data-providers.md).

| Data layer | Реальное представление | Owner | Текущая граница |
| --- | --- | --- | --- |
| Identity in world | ordinary server-assigned `WorldObject::uid` | server | separate UI `uuid` string is not authoritative |
| Scientific discriminator | `metasiberia_scientific_object_v1` first line | Qt settings parser | server opaque |
| Scientific schema | flat JSON values in `WorldObject::content` | Qt client | total content max 10 000 bytes |
| Scientific status/provenance | `schema_version`, `load_status`, `data_origin`, `provenance_*`, provider/parser/cache version and checksum root fields | Qt client | PubChem molecule values are populated by WIP provider path; other providers only valid after real adapter/import path |
| Scientific tables | text `atom/bond/point/value/property` fields | Qt client | ad-hoc formats, individual clamp != total bound |
| Visual derivative | materials + temporary OBJ/MTL -> checksum `.bmesh` | Qt client + generic object/resource flow | code path confirmed; runtime/reconnect untested |
| Source references/cache | local path/URL/database/query/code/prompt descriptors; PubChem cache key/path and image cache path | Qt client + local cache dir | PubChem molecule cache implemented locally; unsupported sources must not mutate scientific data |
| Built-in sample data | explicit local Caffeine/Water atom/bond tables | Qt client | sample/provenance only, not PubChem/provider data |
| Scientific selection/measurements | selection mode/state, selected atom ids, bond index, compact measurement JSON | Qt client | atom/bond scene ray picking and native viewport; server stores descriptor opaquely |
| Scientific labels/legend | label mode/colour/scale/count/distance/runtime status; active element counts | Qt client | rendered in native molecule viewport; no `ObjectType_Text` child lifecycle or SDL/Web parity |
| Scientific classification/catalog | provider/computed/user classification strings, favorite; recent/search history in QSettings | Qt client | provider vs heuristic vs user data explicitly separated; global catalog index remains WIP |
| PubChem lazy sections | section cache manifest in marker; response bytes in local hashed cache | Qt client local cache | Summary/Structure immediate; Images/PUG View sections on demand; large response bodies are not stored in marker |
| Scientific animation | animation flags/frame metadata/runtime status | Qt client | Rotation/Spin active only in native molecule viewport; other animation modes WIP |
| Scientific physics intent | collision/solid/trigger/selectable/movable/gravity/motion/shape/layer/mass/friction/restitution fields | Qt client + existing `WorldObject` fields | collidable/dynamic/sensor/mass/friction/restitution applied; others stored as schema intent |
| AI descriptor | provider/model/endpoint/use-user-credentials in content | Qt client | no provider call |
| AI secret | QSettings `scientificObjectEditor/aiApiKey/<provider>` | local user profile | not object content; storage security unreviewed |
| Persistence | generic WorldObject network/disk serialization | existing server/state | no scientific migration/validation |

Types shown in combo boxes (protein, GIS, volume, medical, etc.), non-PubChem source lists (RCSB/NCBI/EMDB/etc.) and file filters are declared vocabulary/status entries, not evidence of parsed or downloaded datasets. PubChem molecule search/load is the first WIP online provider path and still requires owner runtime verification. Future provider/storage rules are centralized in [scientific-data-providers.md](scientific-data-providers.md). Отдельных committed scientific `.csv/.nc/.h5/.fits/.geojson/.shp/.parquet/.npy` datasets не найдено.

## Generated outputs

| Output | Producer | Rule |
| --- | --- | --- |
| Native EXE/DLL/runtime tree | CMake/copy scripts | generated; exclude normal analysis |
| Web JS/WASM/data/cache-hashed HTML | Emscripten scripts | build output; do not hand-edit |
| Scientific molecule OBJ/MTL/bmesh | Scientific temp generator + generic model conversion | OBJ/MTL transient; `.bmesh` checksum resource is intended durable derivative, runtime untested |
| Procedural tree OBJ/bmesh | `TreeGenerator` + generic model conversion | OBJ transient; checksum `.bmesh` and texture URLs are durable generic resources; manual reconnect untested |
| EZ-Tree presets/textures/licenses | `resources/tree_assets/` copied by runtime pipeline | version-controlled input assets; texture bytes immutable after checksum URL creation |
| Site captures/Figma inputs | capture tools | may contain auth/user data; scrub |
| VIVE media/sync state | local helpers | personal/task-only |
| Map tiles/progress | server/services | separate OSM and rendered-world pipelines |

## Secrets and sensitive data

Tracked high-confidence private keys/token files were not found by prior path-only audit; это не security proof. Manual review candidates include credential/auth flows, external integrations, map maintenance, site capture state, fuzz seeds, local backups and current Scientific AI QSettings storage.

Never copy values into docs/issues/logs. Документировать только path/type/risk/remediation.

## Change rules

- Shared model change: enumerate network and disk readers/writers.
- Bulk scientific data: store as content-addressed resource/stream, not unbounded `content` JSON.
- Resource bytes never mutate under existing checksum URL.
- Runtime state/config/credentials are not edited in ordinary code/docs tasks.
- Generated output is not source of truth.
- Backup/restore/migration require explicit production scope and verified active paths.

Связанные документы: [architecture.md](architecture.md), [component-relations.md](component-relations.md), [current-state.md](current-state.md).

Обновлять карту при новом persisted type, data store, secret location, generated stage или scientific schema/resource change.
