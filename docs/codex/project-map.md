# Карта проекта Metasiberia

Назначение: главная карта продуктов, подсистем и физических owners. Это не копия `tree`.

Проверено: 2026-07-10 по Git tree, CMake и entry points; Scientific Object Editor отражён как WIP текущего рабочего дерева.

## Карта основных подсистем

| Подсистема | Логическая роль | Физический owner | Entry/contract | Статус | Куда дальше |
| --- | --- | --- | --- | --- | --- |
| Scientific Object Editor | создание и редактирование scientific metadata/data/preview | `gui_client/ScientificObjectEditor.*`, `ScientificObjectSettings.*`, `MainWindow.*` | marker `metasiberia_scientific_object_v1` в `WorldObject::content` | официальный WIP; current implementation Qt-based | [data-map](data-map.md#scientific-object-wip), [rules](development-rules.md#scientific-object-editor-wip) |
| Native Client | 3D UI, world interaction, editors, render/audio/XR/network | `gui_client/` | Qt `MainWindow.cpp::main`, SDL `SDLClient.cpp::main` | основной | `gui_client/AGENTS.md`, [architecture](architecture.md#native-client) |
| Web Client | browser delivery общей client codebase | `gui_client/` + `webclient/webclient.html` | Emscripten target `gui_client`, WSS | реализован, не отдельный app | `webclient/AGENTS.md` |
| Public Website | marketing landing | source отсутствует в repo | `metasiberia.com` | внешний/неизвестный | [system overview](system-overview.md#продукты-и-поверхности) |
| Server Website | public/account/auth site | `webserver/`, `webserver_public_files/`, `webserver_fragments/` | `WebServerRequestHandler`, `MainPageHandlers` | основной source | `webserver/AGENTS.md` |
| Admin Panel | privileged server-rendered management | `webserver/AdminHandlers.*` | routes `/admin*` | одна встроенная панель | [architecture](architecture.md#httpwebsocket-server-server-website-и-admin-panel) |
| Realtime Server | authoritative worlds/users/resources/scripts/persistence | `server/` | `Server.cpp::main`, `WorkerThread` | основной | `server/AGENTS.md` |
| HTTP/WebSocket Server | HTTP(S), static/data routes и WSS upgrade | `webserver/` + external glare-core web core | `WebServerRequestHandler` | встроен в `server` | `webserver/AGENTS.md` |
| Shared Engine/contracts | domain models, protocol, serializers + external engine foundation | `shared/`; external `glare-core`, `winter` | `Protocol.h`, `WorldObject.*`, CMake env paths | критический общий слой | `shared/AGENTS.md`, [relations](component-relations.md) |
| Pipeline | configure/build/assets/web/release/ops | CMake, `scripts/`, `resources/`, `systemd/`, CI | per-stage scripts/manifests | mixed active/historical | `scripts/AGENTS.md`, [build guide](build-and-test.md) |

## Подтверждённые сквозные подсистемы

| Подсистема | Owners | Основная граница |
| --- | --- | --- |
| World/object editing | client editors + shared models + server handlers | UI change не заменяет authoritative validation |
| Chat/private messages/attachments | client chat UI/history, `shared/Protocol.h`, server routing | private recipient UID первичен; redesign partial |
| Resources/media | `ResourceManager`, upload/download threads, HTTP handlers | content checksum identity; metadata/bytes раздельны |
| Map/geospatial | `MapWorld*`, MiniMap, OSM tile handler/cache, maintenance scripts | world Mercator scale != MiniMap zoom |
| Server-side scripting | `ServerSideScripting`, `SubstrataLuaVM`, evaluators | server-owned execution/permissions/state |
| XR/OpenXR | `gui_client/XR*`, `MainWindow` lifecycle | native optional, default OFF, external SDK |
| Particles | `ParticleEmitterSettings`, Object Editor, `ParticleManager`, `GUIClient` | marker/content-driven client subsystem; active changes |
| Authentication/accounts | client login, Login/Account handlers, web sessions | native/web flows разделены, shared identity state |
| Screenshot/map rendering | `screenshot_bot` + `gui_client --screenshotslave` | bot connection + localhost renderer |
| Asset/build/release/operations | CMake, scripts, assets, CI, systemd, external runtime | stages и side effects проверяются отдельно |

## Карта репозитория

| Путь | Назначение | Открывать когда |
| --- | --- | --- |
| `CMakeLists.txt`, `cmake/`, `functions.cmake` | общий build graph/options | target, dependency, compiler/configure |
| `gui_client/` | native/web client code, UI, editors, rendering, XR | gameplay/UI/render/network/editor |
| `server/` | realtime server, authoritative state, scripting, persistence | protocol/state/permissions/server lifecycle |
| `webserver/` | embedded routes/site/admin/WS | route/auth/admin/resource/site |
| `shared/` | wire/model/disk contracts | protocol/serialization/domain model |
| `webclient/` | Emscripten HTML shell | browser loading/preload/cache |
| `webserver_public_files/` | CSS/JS/images Server Website | frontend behavior/appearance |
| `webserver_fragments/` | server-rendered fragments/config | content/help/about/generic pages |
| `resources/`, `shaders/` | runtime client assets | lookup/render/packaging |
| `source_resources/`, `icons/` | editable asset masters | asset authoring/provenance |
| `server_dist_resources/` | server seed assets | bootstrap/staging task |
| `screenshot_bot/` | screenshot/map/gear helper | screenshot pipeline |
| `audio/`, `qt/`, `ethereum/` | reusable project-owned source modules | stack/evidence указывает сюда |
| `libs/` | aggregate dependency library target | build/dependency problem |
| `scripts/` | build/copy/release/ops/analysis helpers | workflow; сначала классифицировать side effects |
| `systemd/` | service/timer templates | infrastructure task с разрешением |
| `.github/workflows/` | configure-smoke CI | CI/configure issue |
| `testfiles/` | fixtures/fuzz corpus | конкретный test/format regression |
| `tools/site_capture/`, `tools/figma/` | site capture/Figma import tools | website design workflow |
| `docs/` | permanent, operational, plan, release и Wiki docs | перед architecture/build/ops/contracts task |

## Вспомогательные и task-only области

| Путь | Роль | Текущий статус |
| --- | --- | --- |
| `browser_process/` | CEF subprocess | conditional |
| `backup_bot/` | historical backup client | root-disabled/unconfirmed |
| `cv_bot/` | computer-vision helper | experimental/unconfirmed |
| `lightmapper_bot/` | lightmap helper | root-disabled/unconfirmed |
| `stress_test/` | load client | root-disabled/unconfirmed |
| `installer/` | historical C++ installer | own CMake, root-unwired |
| `local_backups/` | local recovery archive | не canonical |
| `emscripten_build/`, `emscripten_build2/` | tracked generated caches | не source of truth |

## Границы обычного чтения

Всегда исключать: `.git/`, local build/output, `node_modules/`, generated JS/WASM/data, binary/media contents, secrets, credentials и production state.

Обычно исключать: vendored `resonance-audio/`, `opus/`, `secp256k1-master/`, `miniaudio/`, `minimp3/`, `rtaudio/`, dependency implementation в `libs/`, старые release notes и archives — пока evidence не указывает туда.

Не исключать из-за размера: `gui_client/`, `server/`, `webserver/`, `shared/`, website assets и `scripts/`, если они входят в producer/consumer map. Большие `.cpp` читать диапазонами.

## Особо чувствительные пути

- Любые уже dirty/untracked файлы пользователя.
- `shared/Protocol.h`, `WorldObject.*` и другие serializers.
- `server/ServerWorldState.*` и runtime state documentation.
- `ScientificObjectSettings.*`: WIP marker/schema и общий 10 KB content limit.
- version headers, publish/deploy/backup/restore scripts.
- Wiki `_Sidebar.md`/`_Footer.md` и external publishing workflows.

## Навигация

Основной маршрут по задаче: [project-index.md](project-index.md) -> [search-guide.md](search-guide.md) -> ближайший `AGENTS.md` -> минимальный source range.

Обновлять карту при добавлении/удалении top-level owner, нового deployable, нового data store или переносе subsystem boundary.
