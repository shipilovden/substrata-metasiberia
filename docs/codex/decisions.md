# Architecture Decision Records

Назначение: реестр решений, которые нельзя безопасно выводить заново в каждой задаче.

Проверено: 2026-07-10. Историческая мотивация указывается только когда подтверждена; иначе «Почему» — инженерное обоснование, выведенное из текущего design, а не выдуманная история.

## ADR-001 — Source aggregation вместо package isolation

- **Статус:** принято, наблюдается в code/build graph.
- **Контекст:** `shared`, `webserver`, `audio`, `qt`, `ethereum` не образуют независимые libraries/services.
- **Решение:** executable targets напрямую агрегируют общие source modules; `libs` остаётся отдельным dependency library target.
- **Почему:** текущая CMake-композиция связывает consumers на compile time и не предоставляет независимого version/deploy boundary.
- **Что изменилось:** документация больше не приравнивает каталог к target или service.
- **Последствия:** shared source/config change проверяется во всех физических consumers; directory ownership не ограничивает impact.
- **Подтверждение:** root, `gui_client`, `server`, `libs` CMake.

## ADR-002 — Одна client codebase для native и web

- **Статус:** принято.
- **Контекст:** Qt и SDL имеют разные entry/UI paths; Emscripten использует SDL/common client.
- **Решение:** Native и Web Client считаются разными surfaces одной C++ codebase/target, а `webclient/` — browser shell.
- **Почему:** gameplay/world/network logic физически переиспользуется; второй backend/client implementation отсутствует.
- **Что изменилось:** отклонена модель «Web Client — самостоятельная реализация» из раннего MCOS текста.
- **Последствия:** common changes могут затронуть native/web; Qt-only code требует boundary; Web имеет собственный packaging/runtime check.
- **Подтверждение:** `USE_SDL`/`EMSCRIPTEN` CMake branches, `MainWindow.cpp`, `SDLClient.cpp`, `webclient.html`.

## ADR-003 — HTTP/WS/Website/Admin встроены в server process

- **Статус:** принято.
- **Контекст:** `Server.cpp` запускает web listeners; server target компилирует `webserver/`.
- **Решение:** Realtime, HTTP/HTTPS/WebSocket, Server Website и Admin — логические подсистемы одного executable/process и общего state.
- **Почему:** отдельного executable/API service/admin SPA нет; handlers напрямую используют `ServerAllWorldsState`.
- **Что изменилось:** документация разделяет logical responsibilities, но не выдумывает deployment boundaries.
- **Последствия:** web/admin mutation анализируется с server locks/auth/state и deployируется вместе с server binary плюс отдельно загружаемыми assets.
- **Подтверждение:** `server/CMakeLists.txt`, `Server.cpp`, `WebServerRequestHandler.cpp`.

## ADR-004 — Один versioned binary protocol поверх TLS и WebSocket

- **Статус:** принято.
- **Контекст:** Native использует TLS TCP/UDP; Web Client входит через WebSocket.
- **Решение:** binary protocol v62 и `WorkerThread` semantics общие; transport не создаёт второй application protocol.
- **Почему:** WebSocket upgrade передаётся в тот же handler, а shared message IDs/models уже являются общим contract.
- **Что изменилось:** HTTP/WS больше не описывается как отдельный generic WebSocket API без evidence.
- **Последствия:** payload/ID change требует all peers, bounds, capability/version и cross-transport проверки.
- **Подтверждение:** `shared/Protocol.h`, `MessageUtils.h`, `handleWebSocketConnection`, `WorkerThread`.

## ADR-005 — Custom binary record database для authoritative state

- **Статус:** принято.
- **Контекст:** server сериализует record/model state в `server_state.bin`; large bytes находятся в directories.
- **Решение:** persistence считается versioned custom binary contract, не SQL/ORM.
- **Почему:** readers/writers/migrations реализованы в server code; SQL schema/service отсутствуют.
- **Что изменилось:** термины `db_dirty` больше не интерпретируются как SQL database.
- **Последствия:** model change может сделать snapshots нечитаемыми; нужны old reader/migration, copied fixture и backup/restore plan.
- **Подтверждение:** `ServerWorldState::readFromDisk`, `serialiseToDisk`, migrations.

## ADR-006 — Content-addressed resources

- **Статус:** принято.
- **Контекст:** metadata и binary bytes разделены; URLs включают content checksum.
- **Решение:** checksum URL является immutable identity resource content.
- **Почему:** upload/download/cache/HTTP delivery зависят от стабильного соответствия URL и bytes.
- **Что изменилось:** resource fixes должны создавать новый identity, а не подменять старый hash.
- **Последствия:** fallback/request/cache consumers проверяются вместе; metadata/state и file storage должны оставаться согласованными.
- **Подтверждение:** `ResourceManager`, resource handlers, upload/download code.

## ADR-007 — Website content disk-loaded и deployable отдельно от binary

- **Статус:** принято.
- **Контекст:** `WebDataStore` загружает/compresses public files, fragments и Web Client output; watcher поддерживает reload.
- **Решение:** website source, generated Web Client, deployed copies и server executable являются отдельными stages.
- **Почему:** assets не вшиты в binary и могут обновляться независимо.
- **Что изменилось:** server build больше не считается автоматической публикацией website.
- **Последствия:** runtime path/config/cache hashes и drift проверяются отдельно; deploy требует scope/approval.
- **Подтверждение:** `WebDataStore.*`, watcher, server config, website directories.

## ADR-008 — Admin privilege через special god user

- **Статус:** принято как current behavior; пересмотр потребует отдельного security/identity design.
- **Контекст:** одна server-rendered `/admin*` panel; role model не найден.
- **Решение:** current admin guard основан на `UserID == 0`.
- **Почему:** это фактический центральный privilege check текущих handlers.
- **Что изменилось:** документация не называет обычный login admin permission и требует guard на каждом privileged route.
- **Последствия:** identity/bootstrap/role change имеет большой радиус; пропущенный guard — security defect.
- **Подтверждение:** `LoginHandlers::loggedInUserHasAdminPrivs`, `AdminHandlers`, `UserID`.

## ADR-009 — Qt 5 canonical, Qt 6 experimental

- **Статус:** принято до отдельной migration.
- **Контекст:** configs pin Qt 5; source имеет compatibility guards; current Qt6 wrapper/path отсутствует.
- **Решение:** поддерживаемый canonical local path — Qt 5; Qt 6 не объявляется complete.
- **Почему:** source compatibility без configure/link/runtime evidence недостаточна.
- **Что изменилось:** удалено утверждение о полноценной dual support.
- **Последствия:** Qt5 regression check обязателен; Qt6 work требует end-to-end migration criteria.
- **Подтверждение:** config scripts, client CMake, Qt6 migration document.

## ADR-010 — OpenXR optional и native-only

- **Статус:** принято/экспериментально.
- **Контекст:** SDK внешний, option default `OFF`, Emscripten+XR запрещён.
- **Решение:** XR расширяет native client, не становится mandatory client dependency.
- **Почему:** desktop/non-XR path должен работать без runtime/SDK; Web Client не поддерживает этот integration.
- **Что изменилось:** XR fixes не могут использовать camera offset или лишний desktop render как постоянный обход.
- **Последствия:** On/Off compile boundaries, desktop fallback, local-avatar visibility и frame budget являются invariants.
- **Подтверждение:** root/client CMake, `gui_client/XR*`, XR docs.

## ADR-011 — Manual owner-controlled release publication

- **Статус:** принято policy.
- **Контекст:** historical helper может commit/tag/push/upload автоматически.
- **Решение:** version headers + notes + annotated tag; Windows installer проверяет и загружает владелец вручную.
- **Почему:** текущая release policy отделяет подготовку артефакта от необратимой публикации.
- **Что изменилось:** `publish_update.ps1` не считается canonical one-click release.
- **Последствия:** Codex не публикует asset/tag/push без явного задания; headers синхронизируются.
- **Подтверждение:** `docs/RELEASE_PIPELINE.md`.

## ADR-012 — Public Website вне repository boundary

- **Статус:** принято как current evidence boundary.
- **Контекст:** repo содержит Server Website для `vr`, но source `metasiberia.com` не найден.
- **Решение:** Public Website, Server Website и Admin Panel документируются раздельно.
- **Почему:** разные origin/назначение/source ownership; смешение создаёт ложные deployment expectations.
- **Что изменилось:** `webserver_*` больше не называется source Public Website.
- **Последствия:** изменение landing требует external source/owner; repo patch влияет только на подтверждённые Server Website assets/routes.
- **Подтверждение:** repo tree, README/operational docs, web routes.

## ADR-013 — Scientific Object v1 использует generic WorldObject envelope

- **Статус:** provisional WIP; не committed baseline.
- **Контекст:** Qt WIP нужен для совместного world placement/persistence без нового server protocol/type.
- **Решение:** discriminator `metasiberia_scientific_object_v1` + JSON хранится в `WorldObject::content`; create/update/persistence generic.
- **Почему:** текущая реализация переиспользует selection, transform, permissions, network и state; special server/shared implementation отсутствует.
- **Что изменилось:** Scientific Object признан официальным WIP, но не объявлен отдельным shared/server type или готовой platform.
- **Последствия:** 10 KB content limit, opaque server validation, текущая интерпретация в Qt client и marker compatibility; будущий Web/SDL editor и bulk data/resource/execution требуют отдельного design.
- **Подтверждение:** `ScientificObjectSettings.*`, `ScientificObjectEditor.*`, `MainWindow.*`, `WorldObject::MAX_CONTENT_SIZE`, отсутствие marker в server/shared.

## ADR-014 — Иерархия знаний Code -> docs/codex -> MCOS -> AGENTS

- **Статус:** принято policy.
- **Контекст:** длинные session instructions и повторные audits дорожают и быстро устаревают.
- **Решение:** code определяет факты; `docs/codex` хранит актуальные знания; MCOS задаёт инженерный стандарт; AGENTS — компактные loaders.
- **Почему:** один canonical home для знания уменьшает дубли, конфликты и token cost.
- **Что изменилось:** AGENTS освобождаются от книги проекта; documentation portal становится основной памятью.
- **Последствия:** structural knowledge update идёт снизу вверх; AGENTS меняются только при route/behavior rule change.
- **Подтверждение:** MCOS Appendix A/F/H и Documentation Migration Phase 2/3.

## Правило добавления ADR

Добавлять запись только для решения с устойчивым cross-task последствием. Обязательные поля: статус, контекст, решение, почему, что изменилось, последствия и evidence. Если историческая причина неизвестна, помечать rationale как вывод из текущего design.

Связанные документы: [architecture.md](architecture.md), [component-relations.md](component-relations.md), [documentation-index.md](documentation-index.md).
