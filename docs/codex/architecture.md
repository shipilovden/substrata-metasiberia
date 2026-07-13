# Архитектура Metasiberia

Назначение: каноническое описание логических слоёв, физических границ, contracts и правил расширения.

Проверено: 2026-07-10 по CMake, entry points, protocol/model serializers, server routing/state и текущему Scientific Object WIP. Production topology приведена только как документированный snapshot.

## Архитектурные принципы, наблюдаемые в коде

1. Один client codebase обслуживает native и web.
2. Один server process объединяет realtime, HTTP/HTTPS/WebSocket, state и persistence.
3. Общие C++ sources агрегируются в executable targets; каталог не гарантирует library/deployment isolation.
4. Server authoritative: client editors предлагают изменения, server повторно проверяет login, permissions, placement и read-only state.
5. Wire и disk compatibility пересекаются через `shared` models.
6. Resources идентифицируются содержимым, а source/build/deploy stages разделены.

Архитектурные решения и последствия: [decisions.md](decisions.md).

## Логические слои

| Слой | Ответственность | Основные owners |
| --- | --- | --- |
| Presentation | Qt/SDL UI, editors, browser shell | `gui_client/`, `webclient/` |
| Interaction/render | world interaction, OpenGL, audio, map, XR, particles | `GUIClient.*`, специализированные client modules |
| Transport | TLS TCP, UDP, WebSocket, HTTP/HTTPS | client network threads, `ListenerThread`, `UDPHandlerThread`, `webserver/` |
| Application | authoritative protocol handlers, scripts, bots, routes | `server/`, `WorkerThread*`, handlers |
| Domain/contracts | IDs, models, message framing, serialization | `shared/` |
| State/data | world state, record DB, resources/media | `ServerWorldState.*`, runtime directories |
| Pipeline/operations | configure, build, assets, services, release | CMake, `scripts/`, `systemd/`, CI |

## Physical build и deployment boundaries

- Root CMake включает `libs`, `gui_client`, `server`, `screenshot_bot` и conditional `browser_process`.
- `server/CMakeLists.txt` компилирует `server/`, `webserver/`, выбранные `shared/`, external web core и engine sources в один executable.
- `shared`, `audio`, `qt` и `ethereum` — source modules, не самостоятельные library targets.
- `libs` — единственный явно объявленный CMake library target.
- `webclient` — output mode target `gui_client`, а не самостоятельный backend.
- Website assets загружаются с диска; обновление binary не гарантирует обновление assets.

Следствие: изменение `shared`, route/state contract или общей client logic требует проверки всех физических consumers, даже если изменён один каталог.

## Native Client

- Qt entry: `MainWindow.cpp::main()`, compile-time `USE_SDL=OFF`.
- SDL entry: `SDLClient.cpp::main()`, `USE_SDL=ON`; Emscripten использует этот/common path.
- `GUIClient` владеет общей world/network/resource/render orchestration.
- `MainWindow` — Qt shell, lifetime/actions/docks; `SDLUIInterface` — альтернативный UI boundary.
- Editors работают с `WorldObject`, materials, parcels, world settings, bots, gear, shaders, particles, WIP scientific objects и WIP procedural tree objects.
- CEF optional; OpenXR native-only и default `OFF`.

## Scientific Object Editor (WIP)

### Физическая интеграция

- Текущая UI-реализация основана на Qt: `ScientificObjectEditor.*`, `ScientificObjectSettings.*`.
- MOC/source registration: `gui_client/CMakeLists.txt` в текущем dirty tree.
- Owner/lifecycle: `MainWindow` создаёт widget, подключает `objectChanged` и `objectTransformChanged`, переключает его вместо обычного Object Editor.
- Создание: `MainWindow::on_actionAddScientificObject_triggered()` создаёт generic unit-cube object и отправляет обычный `Protocol::CreateObject`.

### Data envelope

- Тип на server/shared: `WorldObject::ObjectType_Generic`.
- Discriminator: первая строка `metasiberia_scientific_object_v1`.
- Payload: JSON в `WorldObject::content`.
- Update: обычный `ObjectFullUpdate`; server не интерпретирует scientific JSON.
- Persistence: существующая serialization `WorldObject`; marker остаётся `v1`, schema имеет отдельное поле `schema_version`, полноценного migration framework пока нет.

### Реализованная client-side логика

- metadata, transform, source/status/provenance descriptors, data tables, visualisation, physics, measurements, animation/simulation flags, AI descriptors и custom properties;
- clamping строк/чисел при parse/serialize, payload size guard и preservation unknown root JSON fields;
- source status path with explicit built-in Caffeine/Water samples и prompt-to-template code generation без network AI call;
- molecule atom/bond parser и generation OBJ/MTL во временный каталог;
- generic `objectEdited()` conversion generated model в checksum-addressed `.bmesh` resource;
- material preview, включая CPK-like element colours и existing WorldMaterial emission/glow.

### Ограничения и архитектурный долг

- `WorldObject::MAX_CONTENT_SIZE = 10000`; editor guard добавлен, но build/runtime проверка не выполнялась.
- Внешние database adapters, file parsers и AI provider calls отсутствуют.
- Generated Python/JS/Lua/C#/C++ не исполняется подтверждённым sandbox/runtime.
- Phase 1.2 labels/legend/selection/measurements and Rotation/Spin работают в native molecule viewport; Qt scene input выполняет selected-object atom/bond ray tests. Cross-client `ObjectType_Text` children, per-atom scene meshes and non-rotation animation adapters не подключены.
- Temporary OBJ/MTL являются intermediate; generic conversion/resource code path существует, но end-to-end upload/reload/reconnect runtime не проверен.
- AI key сохраняется provider-specific ключом в QSettings; он не входит в object JSON, но security/storage review не выполнен.
- Scientific marker сейчас распознаёт Qt WIP; server/shared не интерпретируют schema, а SDL/Web UI и специальные tests пока не реализованы. Это текущий статус, не запрет будущей поддержки.

До исправления этих границ нельзя объявлять universal scientific import, shared scientific model или production-ready AI/simulation.

## Procedural Tree Editor (WIP)

Текущая интеграция основана на Qt и переиспользует generic object contract:

```text
WorldObject::ObjectType_Generic
  content = "metasiberia_tree_object_v1\n" + JSON seed/TreeParams
  model_url = generated checksum-addressed .bmesh derivative
```

`TreeParams`, `TreePresets`, `TreeSerialization`, `TreeGenerator`, `TreeObject` и `TreeEditorPanel` находятся в `gui_client/`. Add Tree создаёт Oak preset с random seed, генерирует OBJ, конвертирует его в `.bmesh` resource URL перед `CreateObject`, а editor пересобирает mesh через обычный `MODEL_URL_CHANGED`/`objectEdited` path. Server сохраняет tree JSON как непрозрачный `WorldObject::content`; отдельного shared/server tree type, protocol message, runtime forest system, GLB export, wind physics и SDL/Web parity пока нет.

## Client-server protocol

- Contract: `shared/Protocol.h`, current version `62`; message framing — `MessageUtils.h`.
- Native: TLS framed connection на `7600`; UDP `7601` для latency-sensitive traffic.
- Web: browser загружает artifacts по HTTP, затем WebSocket upgrade приходит в тот же `WorkerThread` и использует тот же binary protocol.
- Upload/download используют отдельные connection types; resource delivery также доступна по HTTP.
- Backward compatibility строится на protocol/model versions, capabilities и безопасном чтении полей.

Любой новый message требует producer, consumer, bounds/permissions, compatibility strategy и проверки обоих peers. Scientific WIP сейчас намеренно переиспользует общий object contract и не добавляет message ID.

## Realtime Server и persistence

`Server.cpp::main()` запускает listeners, workers, world maintenance и embedded web listeners. `ServerAllWorldsState` владеет worlds, users, resources, sessions, screenshots, photos, bots, gear, events и map metadata под общими locking contracts.

`server_state.bin` — custom versioned binary record database. Binary resources/photos/screenshots находятся в directories, metadata — в state. SQL schema, ORM и отдельный migration service не найдены.

Критические зоны: `WorldStateLock`, blocking I/O рядом с lock, `WorkerThread`, UDP/listeners, scripting/HTTP workers, resource manager mutex, routing/broadcast и disk compatibility.

## HTTP/WebSocket Server, Server Website и Admin Panel

`WebServerRequestHandler` маршрутизирует GET/POST/static/resource/map/WebSocket requests. Handlers напрямую используют тот же `ServerAllWorldsState`.

| Поверхность | Граница |
| --- | --- |
| `/`, account/auth/public pages | Server Website |
| `/admin*` | authenticated privileged Admin Panel |
| `/webclient` | Emscripten artifact delivery |
| resource/photo/screenshot/map routes | file/data interfaces |
| WebSocket upgrade | transport к обычному realtime `WorkerThread` |

Отдельного REST service, OpenAPI contract, admin SPA или standalone webserver executable нет. Admin privilege в текущем коде проверяется через special god user `UserID == 0`; изменение identity/roles затрагивает все admin guards.

## Websites вне process boundary

| Имя | Статус |
| --- | --- |
| `vr.metasiberia.com` | server process + deployed website assets по operational docs |
| `metasiberia.com` | Public Website на внешнем hosting; source в repo не найден |
| `rift.metasiberia.com` | внешний Hyperfy runtime; source в repo не найден |

Не обещать изменение Public Website через patch `webserver_*` без получения внешнего source boundary.

## Shared Engine и dependencies

- Project-owned contracts/models: `shared/`.
- Project-owned reusable modules: `audio/`, `qt/`, `ethereum/`.
- External engine/source trees: `glare-core`, `winter`; не патчить локально как скрытый prerequisite.
- Основные dependencies: Qt 5 canonical, SDL, LLVM, LibreSSL, libjpeg-turbo, Jolt, Luau; CEF/OpenXR/Indigo optional.

## Pipeline

1. Editable assets: `source_resources/`, часть `icons/`.
2. Runtime assets: `resources/`, `shaders/`, `server_dist_resources/`.
3. Configure/build: root/subproject CMake и external dependencies.
4. Native staging: `copy_files_to_output.rb`.
5. Web staging: Emscripten preload/build/cache-hash scripts.
6. Runtime deployment: external server state/release directories and service configuration.

Каждый переход имеет отдельный owner и side effects. Подробности без изменения команд: [build-and-test.md](build-and-test.md).

## Правила расширения

- Новый editor: owner/lifetime -> model/envelope -> CMake/MOC/UIC -> translations -> client update -> server validation -> tests/docs.
- Новый protocol feature: ID/payload -> all producers/consumers -> compatibility -> bounds/permissions -> integration.
- Новый route: router -> handler -> auth/validation/locking -> assets/forms -> route checks.
- Новый persisted field: model version -> old reader/migration -> copied fixture -> backup/restore plan.
- Новый scientific source: adapter contract -> input validation/licensing -> bounded data representation -> portable resource flow -> failure/offline behavior; не добавлять имя в UI как доказательство поддержки.

## Известные разрывы

- `generic_page_config.xml` ссылается на отсутствующий `features.htmlfrag`.
- `server_dist_resources/` staging ownership не найден.
- CI выполняет configure, но не compile/test/runtime.
- Root-disabled tools имеют устаревшие или неподтверждённые build paths.
- Scientific content envelope не соответствует заявленной универсальности UI; generated model resource flow требует runtime validation.
- External Public Website/TheRift source отсутствует в repo.

Подтверждённые gaps, которые не исправляются автоматически, ведутся в [engineering-debt.md](engineering-debt.md).

## Связанные документы

[system-overview.md](system-overview.md) · [component-relations.md](component-relations.md) · [data-map.md](data-map.md) · [decisions.md](decisions.md) · [engineering-debt.md](engineering-debt.md) · [development-rules.md](development-rules.md)

Обновлять документ при изменении process/build boundary, transport/persistence contract, website ownership или Scientific Object data envelope.
