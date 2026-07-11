# Глоссарий MetaSiberia

Назначение: единое место для устойчивых инженерных терминов, которые встречаются в разных подсистемах и документах.

Проверено: 2026-07-10 по `docs/codex` и точечным source checks. Глоссарий не заменяет профильные документы и не является API reference.

## Как пользоваться

- Если термин непонятен, сначала открыть этот файл, затем профильный документ из последней колонки.
- Если термин относится только к одной подсистеме и не повторяется в будущих задачах, не добавлять его сюда.
- Если определение начинает пересказывать код, перенести подробность в профильный документ или source comment.
- При конфликте с рабочим кодом исправить глоссарий.

## Термины

| Термин | Значение | Не путать с | Канонический маршрут |
| --- | --- | --- | --- |
| AGENTS.md | Компактный loader знаний для Codex: порядок чтения, локальные правила, ссылки на `docs/codex`. | Документация архитектуры или источник истины. | [MCOS.md](MCOS.md), [documentation-index.md](documentation-index.md) |
| ADR | Architecture Decision Record: запись устойчивого решения, его причины и последствий. | Журнал изменений, TODO или список идей. | [decisions.md](decisions.md) |
| Admin Panel | Server-rendered privileged routes `/admin*` внутри embedded webserver. | Отдельная SPA или внешний admin service. | [architecture.md](architecture.md), [component-relations.md](component-relations.md) |
| Build boundary | Физическая граница target/dependency в CMake или external toolchain. | Каталогом в repo или logical subsystem. | [architecture.md](architecture.md), [build-and-test.md](build-and-test.md) |
| Client codebase | Общая C++ client implementation, используемая native и web variants. | Двумя независимыми игровыми клиентами. | [architecture.md](architecture.md), [decisions.md](decisions.md) |
| Content-addressed resource | Resource, identity которого определяется bytes/checksum URL. | Mutable file path или обычный attachment name. | [data-map.md](data-map.md), [decisions.md](decisions.md) |
| Current implementation | То, что подтверждено текущим code/build graph или явным runtime evidence. | Roadmap, MCOS-goal, mock UI или planned capability. | [current-state.md](current-state.md) |
| Data envelope | Внешний контейнер данных, который переносится через существующий object/protocol/storage contract. | Новым shared/server type или отдельной database schema. | [data-map.md](data-map.md), [scientific-object-editor.md](scientific-object-editor.md) |
| Deployment boundary | Граница процесса, сервиса, deployed asset stage или production state. | Source directory boundary. | [architecture.md](architecture.md), [project-map.md](project-map.md) |
| Embedded WebServer | HTTP/HTTPS/WebSocket routing, compiled into the `server` executable. | Standalone nginx/node/web service. | [architecture.md](architecture.md), [project-map.md](project-map.md) |
| Engineering Debt | Известная инженерная проблема, которую нельзя или не стоит чинить в текущей задаче. | Разрешением автоматически исправлять всё найденное. | [engineering-debt.md](engineering-debt.md), [development-rules.md](development-rules.md) |
| Generic object envelope | Использование `WorldObject::ObjectType_Generic` + marker/payload для специализированного client-side смысла. | Полноценным domain type в `shared`/`server`. | [scientific-object-editor.md](scientific-object-editor.md), [data-map.md](data-map.md) |
| Marker | Устойчивый discriminator внутри text payload, по которому client распознаёт special content. Для Scientific v1: `metasiberia_scientific_object_v1`. | Server-side type, protocol message ID или schema registry. | [scientific-object-editor.md](scientific-object-editor.md) |
| MCOS | MetaSiberia Codex Operating System: инженерная конституция и правила организации знаний. | Архитектурной документацией проекта. | [MCOS.md](MCOS.md) |
| MiniMap | Client UI map surface with its own zoom/viewport behavior. | Physical Mercator layer of the map world. | [project-index.md](project-index.md), [current-state.md](current-state.md) |
| Native Client | Desktop client variant, primarily Qt 5 canonical path; SDL path also exists. | Web Client shell или отдельный backend. | [architecture.md](architecture.md), [build-and-test.md](build-and-test.md) |
| Payload | Данные внутри existing container: protocol packet body, JSON in `content`, HTTP form body, resource bytes. | Полным объектом или всей database state. | [data-map.md](data-map.md), [development-rules.md](development-rules.md) |
| Persistence | Authoritative server state stored through custom versioned binary records, especially `server_state.bin`. | SQL/ORM или client-local settings. | [data-map.md](data-map.md), [decisions.md](decisions.md) |
| Pipeline | Последовательность source -> generated/build -> staged/runtime -> deployed artifact. | Одной build command. | [architecture.md](architecture.md), [build-and-test.md](build-and-test.md) |
| Placeholder / mock | UI или local template, который показывает будущий workflow без real adapter/runtime. | Реализованной capability. | [current-state.md](current-state.md), [scientific-object-editor.md](scientific-object-editor.md) |
| Producer / consumer | Component, который создаёт/меняет contract, и component, который его читает/использует. | Владельцем каталога. | [component-relations.md](component-relations.md), [search-guide.md](search-guide.md) |
| Public Website | External `metasiberia.com` surface; source в repo не подтверждён. | Server Website at `vr.metasiberia.com`. | [system-overview.md](system-overview.md), [architecture.md](architecture.md) |
| Realtime Server | Authoritative C++ server process for worlds, users, sessions, resources and protocol workers. | Embedded web handlers as a separate process. | [architecture.md](architecture.md), [component-relations.md](component-relations.md) |
| Scientific Object | WIP domain object represented today as generic `WorldObject` with scientific marker + JSON payload. | Завершённой scientific platform или server/shared scientific type. | [scientific-object-editor.md](scientific-object-editor.md) |
| Scientific Object Editor | Current Qt-based WIP UI and settings implementation for editing Scientific Object payloads. | Proof of Web/SDL parity, AI execution or online adapters. | [scientific-object-editor.md](scientific-object-editor.md) |
| Server Website | Website routes/assets served by the embedded webserver for `vr.metasiberia.com`. | External Public Website. | [architecture.md](architecture.md), [documentation-index.md](documentation-index.md) |
| Shared contracts | Protocol IDs, models, serialization and disk/network structures under `shared/`. | A separately deployed shared service. | [architecture.md](architecture.md), [data-map.md](data-map.md) |
| Source owner | Minimal source area that owns a behavior, contract or route. | Sole consumer or deployment owner. | [project-map.md](project-map.md), [search-guide.md](search-guide.md) |
| Token Intelligence | Правила минимального чтения и reuse knowledge, чтобы не повторять полный аудит. | Поверхностной проверкой без evidence. | [token-policy.md](token-policy.md), [search-guide.md](search-guide.md) |
| Web Client | Browser-delivered client variant using the shared C++ client path and `webclient/` shell/assets. | Отдельной gameplay implementation. | [architecture.md](architecture.md), [project-map.md](project-map.md) |
| WebSocket Server | WebSocket upgrade path handled by embedded webserver and routed into the normal realtime protocol worker. | Generic standalone WebSocket API. | [architecture.md](architecture.md), [decisions.md](decisions.md) |
| WIP | Officially recognised work in progress; may be important, but not baseline-ready without stated evidence. | Production-ready или committed baseline. | [current-state.md](current-state.md), [verification-report.md](verification-report.md) |
| WorldObject | Shared world entity model used by client/server for placed objects, including generic objects. | Scientific Object as a separate server type. | [data-map.md](data-map.md), [scientific-object-editor.md](scientific-object-editor.md) |
| `WorldObject::content` | Text payload field with current max read size 10 000 bytes; reused by text, playlists, particle/scientific markers and other content. | Unlimited JSON storage or binary resource storage. | [data-map.md](data-map.md), [engineering-debt.md](engineering-debt.md) |
| `metasiberia_raster_v4` | Current map tile cache namespace used by client and embedded tile route. | Direct OpenStreetMap tile URL. | [current-state.md](current-state.md), [documentation-index.md](documentation-index.md) |
| `PrivateChatMessageID` | Protocol message ID 2001, added in protocol version 62 for private chat. | Plain nickname-only routing. | [current-state.md](current-state.md), [development-rules.md](development-rules.md) |

## Когда обновлять

Обновлять глоссарий, если новый термин встречается в нескольких документах, влияет на routing будущих задач или регулярно вызывает повторный source search. Не добавлять названия классов и функций, если они нужны только для одного локального patch.
