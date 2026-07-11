# Scientific Object Editor: текущая WIP-модель

Назначение: каноническое техническое описание официальной WIP-подсистемы. MCOS Appendix B задаёт целевое направление; этот документ фиксирует только подтверждённое текущим кодом. Каноническая справка по будущим scientific providers/API/formats вынесена в [scientific-data-providers.md](scientific-data-providers.md).

Проверено: 2026-07-11 по dirty working tree после Phase 1.2 Interactive Molecule Information Layer и follow-up world-space overlay fix. Windows/Qt build через `C:\programming\qt_build.ps1` выполнен успешно 2026-07-11 11:41 local time для `Release` и `RelWithDebInfo`, XR Auto resolved to ON; после overlay fix узкий `RelWithDebInfo gui_client` target пересобран успешно. PubChem apply smoke прошёл для `water` и `nicotine`; molecule-information smoke подтвердил русский resolver, label modes, selection states, measurements, lazy classification и 118 periodic elements. Full manual GUI/server-confirm/reconnect flow не выполнялся автоматически. Файлы WIP пока untracked, интеграционные client files modified.

## Статус

**Официальный WIP; current implementation is Qt-based.** Подсистема интегрирована в текущий рабочий код, но не входит в committed `master` baseline и не считается production-ready. Архитектура generic object/marker не запрещает будущий Web/SDL editor.

Build status: **CONFIRMED** for Windows Qt `gui_client` compile/link/runtime-copy artifact validation on 2026-07-11 via `powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1`.

PubChem/runtime status: **CONFIRMED for provider/application/information-layer smoke** on 2026-07-11 via `--scientific_pubchem_apply_smoke` and `--scientific_molecule_info_smoke`. Confirmed Water CID 962, Nicotine CID 89594, PubChem PNG preview/cache, PUG View classification, six Russian aliases, seven label modes, distance/angle/torsion, native periodic table count 118 and local molecule OBJ assignment. This is not a full manual GUI/server/reconnect verification.

## Owners и entry points

| Роль | Путь/symbol |
| --- | --- |
| Settings/model adapter | `gui_client/ScientificObjectSettings.h`, `gui_client/ScientificObjectSettings.cpp` |
| Qt widget | `gui_client/ScientificObjectEditor.h`, `gui_client/ScientificObjectEditor.cpp` |
| Molecule interaction/render overlay | `gui_client/MoleculeViewportWidget.*` |
| Image viewer | `gui_client/ScientificImageViewer.*` |
| Periodic table | `gui_client/PeriodicTable.*` |
| Creation action | `MainWindow::on_actionAddScientificObject_triggered()` |
| Editor selection/lifecycle | `MainWindow::setObjectEditorFromOb`, `objectEditorToObject`, `showObjectEditor` |
| Change propagation | `ScientificObjectEditor::objectChanged/objectTransformChanged` -> existing `GUIClient` editor slots |
| Build registration | `gui_client/CMakeLists.txt` MOC/source lists |
| Generic shared envelope | `shared/WorldObject.h`, `shared/WorldObject.cpp` |
| Generic server handlers | `Protocol::CreateObject`, `Protocol::ObjectFullUpdate` in `server/WorkerThread.cpp` |

## Архитектурная модель

Scientific Object не является новым `WorldObject::ObjectType`.

```text
WorldObject::ObjectType_Generic
  content = "metasiberia_scientific_object_v1\n" + JSON
  model_url = unit cube или generated molecule model
  materials = preview/molecule materials
  transform = обычный WorldObject transform
```

Server сохраняет и передаёт объект как обычный `WorldObject`; special scientific parser/validator на server/shared отсутствует. Это переиспользует существующие permissions, placement, network update и persistence, но оставляет validation/schema ownership на Qt client.

## Подтверждённый lifecycle

1. Add action проверяет parcel creation permissions.
2. Client создаёт generic unit-cube `WorldObject`, marker/JSON и preview material; default physics для Scientific Object — selectable/movable intent, но non-solid/non-collidable.
3. `CreateObject` отправляется обычному server handler.
4. После server confirmation marker выбирает Scientific Object Editor вместо обычного Object Editor.
5. Widget читает JSON, синхронизирует transform и emits existing editor change signals.
6. `toObject()` сериализует content с проверкой `WorldObject::MAX_CONTENT_SIZE`, обновляет material/physics и для поддержанной molecule preview может записать temporary OBJ/MTL, выставляя `MODEL_URL_CHANGED`.
7. Existing `GUIClient::objectEdited()` загружает model, конвертирует non-bmesh в `.bmesh`, создаёт checksum URL, копирует bytes в local resource store и заменяет `model_url`.
8. Generic object update передаёт полный объект server; server повторно проверяет permissions/placement и запрашивает отсутствующий resource обычным dependency flow.
9. `WorldObject` сохраняется в обычном server state.

## Текущая schema v1

Все поля ниже реально объявлены и читаются/пишутся `ScientificObjectSettings`.

| Группа | Поля |
| --- | --- |
| Schema/status | `schema_version`, `load_status`, `load_status_message`, `data_origin`, preserved unknown root JSON fields |
| Identity/metadata | `name`, `scientific_type`, `description`, `source`, `author`, `tags`, `uuid`, `created_time`, `modified_time` |
| Source descriptor | `source_mode`, `file_path`, `source_url`, `online_database`, `online_query`, `online_result_id`, `code_language`, `code_text`, `prompt_text`, `generated_code` |
| Provenance | `provenance_source`, `provenance_identifier`, `provenance_url`, `provenance_author`, `provenance_loaded_at`, `provenance_format`, `provenance_version`, `provenance_license` |
| Provider/cache/model versions | `provenance_checksum`, `molecule_model_version`, `provider_adapter_version`, `parser_version`, `cache_version`, `visualization_settings_version`, `source_data_cache_*`, `image_*`, `conformer_status` |
| Data payload | `data_summary`, `atom_table`, `bond_table`, `point_table`, `value_table`, `property_table` |
| Visualisation | `visualization_mode`, `colour_scheme`, `display_colour`, `material`, radii/width/opacity/scale, label/legend/hydrogen flags, label descriptor/runtime status, `lod_level`, glow/outline/wireframe descriptors |
| Interaction/measurements | selection mode/state, selected atom/bond ids, saved measurement JSON, distance/angle/torsion flags, atom/bond/point counts, dimensions |
| Classification/search | provider/computed/user classifications, favorite, section cache manifest, original/normalized/translated query |
| Animation descriptors | rotation/trajectory/vibration/time-series flags, speed/current/frame count, `animation_runtime_status` |
| Simulation descriptors | enabled, type, notes |
| AI descriptors | provider, model, endpoint, use-user-credentials flag |
| Physics/interaction | `collision_enabled`, `solid`, `trigger`, `selectable`, `movable`, `gravity_enabled`, `physics_motion_type`, `physics_shape`, `collision_layer`, `physics_mass`, `physics_friction`, `physics_restitution` |
| Extension payload | `custom_properties` string intended for JSON/properties |

Parser applies defaults and clamps individual strings/numeric ranges. Unknown root JSON fields are preserved during read/write so future schema additions are not lost by ordinary editing. `catch(...)` returns default settings and optional generic parse error; field-level diagnostics/schema validation are not implemented.

## UI vocabulary != supported capability

Combo boxes enumerate scientific types, source modes, online databases, code languages, visualisation modes, simulation types and AI providers. These lists are design vocabulary, not adapter/runtime registry.

### Реально работает в коде

- editing/serializing fields and transform;
- marker-based selection;
- generic create/update/persistence reuse;
- local API-key field stored per provider in QSettings;
- PubChem Phase 1 molecule search/load path via PUG REST/PUG View endpoint patterns, with explicit CID selection; single/first result is auto-selected for visibility but not auto-loaded;
- PubChem HTTPS transport uses Windows WinHTTP/SChannel because the local QtNetwork build has SSL disabled; HTTPS is not downgraded to HTTP and certificate errors are not ignored;
- PubChem metadata/properties, synonyms, SDF structure loading, PNG image cache/preview, local disk cache, throttling/backoff for HTTP 429/503 and provenance/checksum fields;
- PubChem selected-result application path: `Загрузить CID ... в объект` loads metadata/SDF/PNG, updates fields atomically, changes source from `manual` to `PubChem`, emits a single final `objectChanged`, and lets existing `toObject()`/`GUIClient::objectEdited()` handle molecule model/resource flow;
- deterministic prompt-to-Python starter templates for a few keyword cases;
- source status path that reports `Idle`, `Ready`, `Unsupported` or `Error` instead of silently substituting test data;
- explicit built-in molecule sample catalog for `Caffeine` and `Water`, marked as local sample data rather than PubChem/provider data;
- provenance/status fields for source, identifier, format, version, license and loaded-at timestamp;
- unknown root JSON field preservation for forward-compatible schema editing;
- payload byte-size guard before assigning `WorldObject::content`;
- molecule table parser, basic ball-and-stick/space-fill/wireframe OBJ/MTL generation;
- molecule generated model fingerprint includes atom radius, bond thickness, scale, opacity, hydrogen visibility, colour scheme and wireframe flag, so visual changes can trigger `MODEL_URL_CHANGED`;
- generic model conversion в content-addressed `.bmesh` и existing resource dependency flow;
- CPK-like element colours/materials;
- existing `WorldObject` physics/material fields for collision/dynamic/sensor/mass/friction/restitution and emission/glow.

### Placeholder или не подтверждено

- real ChEBI/ChemSpider/RCSB/AlphaFold/NCBI/Materials Project/OQMD/COD/EMDB/NASA/OSM/etc. network adapters;
- file parsing for PDB/MOL/SDF/XYZ/CIF/CSV/JSON/PLY/LAS/OBJ/STL/glTF/medical/volume formats shown by file dialog;
- URL loading;
- calls to OpenAI/Anthropic/Gemini/OpenRouter/DeepSeek/Ollama/LM Studio;
- execution of generated Python/JavaScript/Lua/C#/C++;
- persistent/cross-client child-object labels through `WorldObject::ObjectType_Text`; Phase 1.2 labels are rendered by the native molecule viewport and by selected-object world-space editor overlays;
- trajectory/vibration/time-series animation and solver integration; only local Rotation/Spin in the native molecule viewport is active;
- scientific simulation, area/volume solvers or large-data streaming;
- export/share API beyond ordinary world-object persistence;
- plugin/registry boundary for disciplines;
- SDL/Web Client UI parity.

Только button «Сгенерировать код» вызывает local prompt-to-template generator. «Создать объект», «Объяснить» и «Оптимизировать» сейчас лишь emit `objectChanged` и не реализуют отдельную AI/processing operation.

## Source/provider truth table

Current Qt editor has a first real molecule provider path for PubChem. Other provider entries are status vocabulary unless listed as explicit built-in sample.

Будущие real adapters должны сверяться с [scientific-data-providers.md](scientific-data-providers.md). Этот документ остаётся источником текущего implementation status, а provider reference — источником API/format/provenance policy.

| Category | UI/source entries | Current status |
| --- | --- | --- |
| Built-in molecule samples | Built-in sample catalog; query `Caffeine` or `Water`; type `molecule` | implemented local sample data, provenance `built-in:*`; not citable external data |
| Molecules / PubChem | PubChem PUG REST/PUG View endpoint patterns; name/CID/SMILES/InChI/InChIKey/formula heuristic search; SDF/PNG/properties | implemented in Qt WIP for interactive search, visible CID selection, metadata, SDF parse, PNG preview/cache, provenance and object application; headless network smoke and QWidget apply smoke confirmed `water`/`nicotine`; full manual GUI/server/reconnect flow still requires owner verification |
| Molecules / other providers | ChEBI, ChemSpider | unsupported/planned; no network/API path |
| Proteins and nucleic acids | RCSB Protein Data Bank, AlphaFold, NCBI | unsupported/planned; no PDB/FASTA/sequence parser path |
| Crystals/materials | Materials Project, OQMD, Crystallography Open Database, COD | unsupported/planned; no CIF/POSCAR/API-key path |
| Density/volume maps | EMDB | unsupported/planned; no CCP4/MRC parser path |
| GIS/map data | OpenStreetMap, USGS, Natural Earth, Copernicus | unsupported in Scientific Editor; existing MetaSiberia map systems are separate |
| Point clouds | OpenTopography | unsupported/planned; no PLY/LAS/XYZ import path |
| Space/planetary | NASA, NASA Open API, JPL, ESA | unsupported/planned |
| Generic repositories | Zenodo, FigShare, CERN Open Data, GBIF, NOAA | unsupported/planned |

If a source/type pair is unsupported, loading must update status/request state only and must not mutate atom/bond/point data or overwrite provenance of existing data. This rule was added to prevent the previous fixed molecule substitution behavior.

## Phase 1 — Molecules

Current implementation status:

| Capability | Status |
| --- | --- |
| HTTPS transport | implemented on Windows via WinHTTP/SChannel; QtNetwork SSL is disabled in this local Qt build |
| PubChem search | implemented for button-driven interactive requests; query kind is inferred as CID/name/SMILES/InChI/InChIKey/formula |
| Result list | implemented; first/single result is selected visibly; user still explicitly loads via button |
| PubChem metadata/properties | implemented via property JSON and optional synonyms/PUG View title |
| PubChem structure | implemented via SDF 3D request with 2D SDF fallback |
| SDF parser | implemented for V2000 atom/bond tables used by MoleculeModel-like internal tables |
| 2D image | implemented via PubChem PNG endpoint and local cache; source preview loads on CID application, while the Images tab provides fit/zoom/wheel/pan/reset/open/save and source/license status |
| Cache/provenance | implemented locally with response cache files, SHA-256 checksums and provider/parser/cache version fields |
| Visualisation | existing molecule OBJ/MTL path supports ball-and-stick, space-fill and wireframe-like modes from atom/bond tables; QWidget apply smoke confirmed local OBJ assignment for `water` and `nicotine` before GUIClient resource conversion |
| Legend | active CPK legend in native molecule viewport with colour, symbol, full element name and atom count |
| Labels | seven working modes in native molecule viewport: symbol, atom number, element+number, atomic number/mass, formal charge and custom attribute; colour/scale/count/distance controls apply |
| Interactive selection | selected-object scene ray tests and native viewport picking for atoms/bonds; no/atom/multiple/bond/molecule states persist in settings |
| Measurements | distance, angle and torsion from selected atoms; center of mass, bounding dimensions, maximum extent and molecular mass are derived without solver claims |
| Animation runtime | local Rotation/Spin active in native molecule viewport; trajectory/vibration/time-series remain WIP |
| Other scientific types | planned/unsupported; must not use molecule fallback |

## Phase 1.2 — Interactive Molecule Information Layer

Implemented in the current Qt WIP:

- stable atom index/element/coordinates/label/selection/measurement mapping from parsed atom and bond tables;
- atom/bond ray picking in the selected world object and screen picking in the molecule viewport;
- selected-atom yellow highlight and atom labels are mirrored into the 3D world as temporary native editor GL overlays for the selected Scientific Object, so they move with the object and are removed/recreated with selection/editor state;
- atom/bond/molecule context actions, element card integration and periodic-table selection;
- PubChem summary/structure immediate load, lazy Images and PUG View card sections with local cache/status;
- Russian exact-first resolver with an externalizable local alias map for `вода`, `никотин`, `аспирин`, `кофеин`, `этанол`, `глюкоза`; identifiers are not translated;
- provider classification, explicitly marked formula heuristic and user collections/favorites are distinct fields;
- native Periodic Table table/list/3D property graph with 118 records and `Not available` for unsourced physical values;
- simulation controls disabled with `backend unavailable`; no molecular dynamics/CFD/orbital solver is imitated.

Current scope boundary: labels/legend and per-atom highlight are native client information overlays. Atom labels/highlight now appear both in the editor molecule viewport and in the selected object's 3D world-space overlay, but they are not persisted `ObjectType_Text` child objects or separate server entities and do not claim SDL/Web parity. Full owner UI review is still required for world-space visual ergonomics, context menus and reconnect behavior.

## Жёсткие ограничения

### Payload size

`WorldObject::MAX_CONTENT_SIZE` равен **10 000 байт**. Scientific serializer допускает множество отдельных text fields до 8 192 байт, поэтому совокупный UI state может значительно превысить network reader limit. Это текущий correctness blocker для заявленной универсальной data model.

Qt editor теперь измеряет итоговый serialized byte size перед записью в `WorldObject::content` и показывает ошибку вместо отправки oversized payload. Build подтверждён 2026-07-11; payload-overflow runtime behavior остаётся **REQUIRES OWNER-REQUESTED RUNTIME VERIFICATION**. Увеличение общего limit без protocol/state/performance review недопустимо.

### Generated model resource flow

Molecule generation пишет `.obj/.mtl` в `PlatformUtils::getTempDirPath()` и устанавливает local path как `model_url`. Затем общий `GUIClient::objectEdited()` path при `MODEL_URL_CHANGED` загружает mesh, пишет `.bmesh`, вычисляет checksum URL, копирует resource и заменяет local path. Server dependency flow может запросить отсутствующий resource.

Code path compile/link подтверждён Windows/Qt build 2026-07-11; PubChem network smoke проверил HTTPS/parser/image path; QWidget apply smoke подтвердил selected CID -> `WorldObject.content`/local OBJ application. Manual GUIClient/server resource conversion, `.bmesh` upload, reconnect and second-client visibility не выполнялись автоматически. Поэтому остаётся обязательная manual проверка, что resulting `.bmesh`, materials и resource upload корректно переживают второй client и restart. Temporary OBJ/MTL не являются persisted source of truth.

### Data/schema

- Marker содержит `v1`; отдельного migration framework нет.
- Unknown root JSON fields are preserved during ordinary read/write, but nested semantic migrations are not implemented.
- JSON values largely stored as flat strings; tables have ad-hoc line formats.
- Server не валидирует marker/schema.
- `custom_properties` не валидируется как JSON.
- Object metadata `uuid/created_time/modified_time` не связывается с server UID/timestamps автоматически.

### Labels and animation

Label settings store mode, colour, scale, count/distance limits and `label_runtime_status`. Phase 1.2 renders them in `MoleculeViewportWidget` and, for the selected object, through native 3D GL text overlays owned by `GUIClient::selected_ob_vis_gl_obs`. Existing `ObjectType_Text` child-label creation/sync remains intentionally unused, so SDL/Web parity and persistent independent label objects are not claimed. Selected atoms and bonds are mirrored as temporary yellow 3D overlays for the selected object; these are editor overlays, not material edits to the generated molecule mesh.

When a molecule structure is replaced, volatile atom-index-dependent state such as active measurements is cleared and measurement JSON serialization validates atom indices. This prevents stale distance/angle/torsion records from the previous molecule from crashing the editor while the new object data is applied.

Animation settings record `animation_runtime_status`. Rotation/Spin is connected to the native molecule viewport timer and, while enabled in the editor, advances the selected object's transform through the ordinary transformChanged path so the world object visibly spins too. Trajectory/vibration/time-series remain `WIP`; none of these controls creates a molecular simulation.

### Credentials и execution

API key сохраняется в QSettings path `scientificObjectEditor/aiApiKey/<provider>` и не сериализуется в object content. Encryption/OS credential-store integration не подтверждены. До security design нельзя считать это production-ready secret storage.

Generated code currently text-only. Любое будущее execution требует explicit sandbox, time/memory/network/file limits, dependency policy, deterministic output contract и permissions/threat review.

## Compatibility rules

- Не менять marker v1 без reader strategy для существующих objects.
- Unknown/old clients видят generic object; model/material must remain usable без scientific UI.
- Новое поле должно иметь default и bounded serialization.
- Отдельный shared/server scientific type, message ID или storage вводится только через ADR и cross-version plan.
- Large datasets должны переходить в content-addressed resources/streaming; `content` хранит bounded descriptor, а не arbitrary bulk data.
- Visualisation derivative не должен становиться единственным источником scientific data.

## Критерии перехода из WIP

- files committed и narrow build/test успешно выполнены;
- payload size guard build/runtime tested;
- malformed/unknown schema behavior defined;
- generated model conversion/resource upload/reload подтверждены runtime;
- хотя бы один real source/import adapter имеет validation, error/offline and provenance behavior;
- UI clearly distinguishes built-in sample/template from real provider/import operation;
- label and animation runtime adapters either implemented or explicitly disabled in UI;
- credential storage и AI execution boundaries reviewed, если AI включается;
- server compatibility и old-client behavior verified;
- manual Qt flow проверен; SDL/Web scope explicitly supported or declared unsupported;
- docs/current-state/ADR updated.

## Ручная проверка владельца

После разрешения build/runtime проверки: create/edit/reconnect; permission denial; payload near/over 10 KB; malformed JSON; molecule preview after another client/restart; absence of API key in packet/content/log; mock labels; undo/selection/transform; non-scientific generic object regression.

Связанные документы: [architecture.md](architecture.md#scientific-object-editor-wip), [scientific-data-providers.md](scientific-data-providers.md), [data-map.md](data-map.md#scientific-object-wip), [development-rules.md](development-rules.md#scientific-object-editor-wip), [current-state.md](current-state.md#официальный-wip-scientific-object-editor).

Обновлять документ при изменении marker/schema, real adapter/execution, shared/server contract, resource flow или readiness status.
