# Engineering Debt Register

Назначение: постоянный список известных инженерных проблем, которые обнаружены в ходе анализа, но не должны исправляться автоматически в текущей задаче.

Проверено: 2026-07-11 по текущим `docs/codex` и точечным source checks. Этот документ не даёт разрешения менять код, запускать production или расширять scope задачи.

## Правила

- Долг фиксируется только если он подтверждён кодом, документацией или явным указанием владельца.
- Запись должна помогать будущей задаче начать с правильной области, а не быть эмоциональным TODO.
- Исправление долга выполняется только по отдельной задаче или явному разрешению владельца.
- Если проблема является только временной гипотезой, пометить её в verification report, а не в этом register.
- При исправлении не удалять запись бесследно: указать resolution и ссылку на change/документ.

## Категории и приоритеты

| Категория | Значение |
| --- | --- |
| Engineering Debt | Архитектура, contracts, lifecycle, data boundaries. |
| Documentation Debt | Навигация, статус, дубли, weak discoverability. |
| Build Debt | Configure/build/test/release gaps. |
| UX Debt | UI обещает больше, чем реализованный workflow. |
| Performance Debt | Известный риск latency, frame budget, blocking или resource cost. |
| Security Debt | Credentials, auth, permissions, sensitive data, execution safety. |

| Приоритет | Значение |
| --- | --- |
| P0 | Риск потери данных, production breakage или security incident; требует отдельной немедленной задачи. |
| P1 | Блокирует объявление подсистемы production-ready или может регулярно ломать workflow. |
| P2 | Создаёт заметную стоимость сопровождения, но имеет понятный workaround. |
| P3 | Улучшение структуры/ясности; выполнять при ближайшей тематической задаче. |

## Текущие записи

### ED-001 — Scientific payload может превысить общий лимит content

- Категория: Engineering Debt.
- Приоритет: P1.
- Подтверждение: [scientific-object-editor.md](scientific-object-editor.md), [data-map.md](data-map.md), `WorldObject::MAX_CONTENT_SIZE = 10000`.
- Описание: Scientific schema содержит несколько text/table/custom fields, суммарно способных создать локально валидный JSON больше общего лимита `WorldObject::content`.
- Причина: WIP использует generic object envelope без отдельного bounded scientific storage contract.
- Последствия: объект может редактироваться в UI, но не пройти network/persistence read или вести себя несовместимо между clients.
- Текущее смягчение: Qt editor добавил aggregate byte-size guard перед записью `WorldObject::content`; build/runtime не запускались.
- Предлагаемое решение: проверить guard после owner-approved build/runtime и отдельно решить, остаётся ли v1 в `content` или требует resource-backed/typed storage.

### ED-002 — Scientific schema непрозрачна для server/shared

- Категория: Engineering Debt.
- Приоритет: P2.
- Подтверждение: [scientific-object-editor.md](scientific-object-editor.md), [decisions.md](decisions.md).
- Описание: server хранит Scientific JSON как обычный `WorldObject::content` и не интерпретирует marker/schema.
- Причина: v1 сознательно переиспользует generic object lifecycle без нового shared/server type.
- Последствия: server не может проверять scientific invariants, schema migration, discipline-specific permissions или compatibility beyond generic object bounds.
- Предлагаемое решение: оставить v1 opaque до стабилизации модели; при появлении cross-client/server requirements оформить ADR для shared scientific type или typed resource model.

### ED-003 — Scientific UI vocabulary опережает реальные adapters

- Категория: UX Debt.
- Приоритет: P1.
- Подтверждение: [scientific-object-editor.md](scientific-object-editor.md), [current-state.md](current-state.md).
- Описание: UI содержит database/provider/file-format vocabulary. PubChem molecule path реализован как первый online adapter; HTTPS network/parser/image path подтверждён network smoke для `water`/`nicotine`, а QWidget apply smoke подтвердил selected CID -> object application. Full manual GUI/server/reconnect flow не подтверждён. Остальные online adapters, file imports, external AI calls и execution runtime не подтверждены.
- Причина: WIP прототипирует будущий workflow раньше production adapters.
- Последствия: пользователь или будущая сессия Codex может принять placeholder за реализованную capability.
- Текущее смягчение: PubChem molecule path использует visible CID selection, cache/provenance and error status; load button now applies selected CID in one click; HTTPS выполняется через WinHTTP/SChannel вместо QtNetwork SSL-disabled path; unsupported provider не должен мутировать scientific tables; built-in samples явно отделены от provider data.
- Предлагаемое решение: после owner-approved manual runtime проверить UI status behavior, server-confirm/reconnect/resource flow и при реализации каждого provider добавить validation/offline/provenance tests.

### ED-004 — AI credential storage требует security review

- Категория: Security Debt.
- Приоритет: P1.
- Подтверждение: [scientific-object-editor.md](scientific-object-editor.md), [development-rules.md](development-rules.md).
- Описание: Scientific AI API key хранится локально в provider-specific QSettings key и не входит в object/network payload, но production-ready credential policy не определена.
- Причина: WIP использует простой local settings path без утверждённого secret storage design.
- Последствия: риск неправильного обращения с credentials при будущей AI integration, logs, export/import или multi-user machine.
- Предлагаемое решение: отдельная credential-storage task: threat model, approved local secret store, no-log checks, export/import behavior и UI warnings.

### ED-005 — Generated model resource flow требует runtime/reconnect проверки

- Категория: Engineering Debt.
- Приоритет: P2.
- Подтверждение: [scientific-object-editor.md](scientific-object-editor.md), [architecture.md](architecture.md).
- Описание: code path для PubChem/built-in molecule atom/bond tables -> OBJ/MTL -> `.bmesh` -> checksum resource существует через generic object editing. QWidget apply smoke подтвердил local OBJ assignment, но end-to-end `GUIClient::objectEdited()` `.bmesh` conversion/upload/reload/reconnect не проверялся manual runtime flow.
- Причина: build, PubChem network smoke and QWidget apply smoke выполнялись, но full GUI/server/reconnect проверки не запускались.
- Последствия: Scientific molecule visualization может выглядеть реализованной по коду, но иметь runtime gap в resource availability или second-client/reconnect flow.
- Предлагаемое решение: отдельная local/manual runtime task после build permission: create object, generate molecule, verify upload, reconnect and second client. Production/server restart only по явному разрешению владельца.

### ED-006 — ADR пока находится в одном consolidated file

- Категория: Documentation Debt.
- Приоритет: P3.
- Подтверждение: [decisions.md](decisions.md), [verification-report.md](verification-report.md).
- Описание: 14 ADR records находятся в `decisions.md`; отдельный `docs/codex/adr/` пока не создан.
- Причина: на Phase 5 split дал бы много новых файлов с тем же содержанием и риск дублирования.
- Последствия: при росте числа решений один файл может стать тяжёлым для review и immutable history.
- Предлагаемое решение: создать `docs/codex/adr/` при следующей крупной архитектурной задаче или при достижении порога роста; оставить `decisions.md` индексом, а отдельные ADR сделать canonical records.

### ED-007 — Исторические отчёты находятся рядом с current docs

- Категория: Documentation Debt.
- Приоритет: P3.
- Подтверждение: [documentation-index.md](documentation-index.md), [verification-report.md](verification-report.md).
- Описание: `audit-report.md` и `verification-report.md` остаются в основной папке `docs/codex` как snapshot/history documents.
- Причина: migration сохранила простую структуру без archive/history subtree.
- Последствия: будущая сессия может открыть snapshot вместо `current-state.md` или профильного current doc.
- Предлагаемое решение: после стабилизации knowledge system решить, нужен ли `docs/codex/history/` или достаточно явных statuses в portal.

### ED-008 — Часть first-party documents имеет слабую прямую discoverability

- Категория: Documentation Debt.
- Приоритет: P2.
- Подтверждение: [verification-report.md](verification-report.md), [documentation-index.md](documentation-index.md).
- Описание: release notes, admin backlog и temporary Wiki capability plans классифицированы группами, но не все перечислены в portal по filename.
- Причина: портал избегает длинного списка historical files.
- Последствия: редкая задача может потребовать дополнительный `rg --files` вместо прямого перехода.
- Предлагаемое решение: при следующей Wiki/release/docs cleanup выбрать явную classification policy: grouped history, appendix list или dated history subtree.

### ED-009 — Known architecture gaps требуют отдельных задач

- Категория: Engineering Debt.
- Приоритет: P2.
- Подтверждение: [architecture.md](architecture.md), [audit-report.md](audit-report.md).
- Описание: `features.htmlfrag`, `server_dist_resources/` staging ownership, configure-only CI и root-disabled auxiliary targets остаются известными gaps.
- Причина: эти области не входили в docs-only migration и требуют build/runtime/owner checks.
- Последствия: будущая задача по website/build/pipeline может повторно находить те же gaps.
- Предлагаемое решение: открывать отдельную тематическую задачу на конкретный gap; начинать с `project-index.md` -> профильный owner -> точечный source/build manifest check.

### ED-010 — Scientific information overlays не имеют cross-client child-object lifecycle

- Категория: Engineering Debt / UX Debt.
- Приоритет: P1.
- Подтверждение: [scientific-object-editor.md](scientific-object-editor.md), [component-relations.md](component-relations.md), `WorldObject::ObjectType_Text`.
- Описание: Phase 1.2 реализует labels/legend/selection/measurements и Rotation/Spin в native `MoleculeViewportWidget`, selected-object atom/bond ray tests — в Qt scene input path, а selected-object atom labels/highlight — как native editor GL overlays. Persisted `ObjectType_Text` label children, server-persisted separate per-atom scene meshes, SDL/Web parity и trajectory/vibration/time-series runtime не реализованы.
- Причина: существующая text system живёт как отдельный `ObjectType_Text` path; cross-client child ownership и массовый per-atom scene lifecycle требуют protocol/resource/performance design.
- Последствия: native editor показывает информационный слой в viewport и поверх selected 3D molecule, но другой client без Qt overlay видит только molecule mesh; overlay не является persisted scientific child-object contract.
- Текущее смягчение: runtime status различает `interactive_molecule_viewport_active`, `interactive_viewport_rotation_active`, `wip_not_connected` и disabled simulation backend; smoke проверяет Qt render/model state.
- Предлагаемое решение: после owner UX review решить, нужен ли persistent child-object adapter или client-local OpenGL overlay registry с culling/LOD and SDL/Web implementation.

### ED-011 — PubChem UI request lifecycle остаётся синхронным и без cancel UX

- Категория: UX Debt / Engineering Debt.
- Приоритет: P2.
- Подтверждение: [scientific-object-editor.md](scientific-object-editor.md), [build-and-test.md](build-and-test.md).
- Описание: PubChem HTTPS transport исправлен, network/apply smoke подтверждены, selected CID больше не теряется, а старый результат не должен грузиться после изменения query. Однако интерактивный editor path всё ещё выполняет button-driven provider requests синхронно внутри текущего UI action. Есть timeout/retry/status, но нет отдельного cancel button, request queue или полноценного async provider lifecycle.
- Причина: текущая задача была scoped на блокирующий HTTPS runtime error; полноценный async provider registry/cancellation layer требует отдельного UI/state design.
- Последствия: при медленной сети editor может временно ждать ответа; повторный поиск, закрытие editor и server confirmation во время запроса требуют manual проверки.
- Текущее смягчение: WinHTTP timeouts, limited retry on 429/503, query/result matching guard, disabled load button during synchronous load, explicit error statuses and no sample fallback.
- Предлагаемое решение: вынести providers в async worker/registry with request id, cancel/abort, disabled buttons or progress state, stale-response rejection and UI-safe completion callbacks.

### ED-012 — Procedural Tree остаётся client-generated generic object

- Категория: Engineering Debt / Performance Debt.
- Приоритет: P2.
- Подтверждение: [architecture.md](architecture.md), [component-relations.md](component-relations.md), [verification-report.md](verification-report.md).
- Описание: native Qt client теперь faithfully генерирует EZ-Tree geometry v2 и отдельный checksum resource при realtime edit. Server хранит opaque tree JSON; SDL/Web не имеют editor/generator parity, а forest batching/impostors/wind/collision proxy generation отсутствуют. `castShadows`, alpha-test threshold и LOD intent сериализуются, но не имеют отдельного tree renderer contract; `Trunk Only` и `Simplified` collision пока оба используют generic full-mesh collidable path.
- Причина: v1 сознательно переиспользует generic `WorldObject` и общий model/resource pipeline без нового protocol/type/runtime forest subsystem.
- Последствия: одиночные деревья редактируются без server change, но большие леса могут давать лишние mesh uploads, draw calls, physics complexity и platform divergence.
- Текущее смягчение: generator имеет quality caps/branch queue limit, exact 16-preset checks, schema-1 migration, content-addressed RGBA leaf resources, deterministic seed, debounce 180 ms и Release/RelWithDebInfo smokes.
- Предлагаемое решение: после manual create/reconnect/second-client проверки отдельно спроектировать derived-mesh cache by parameter fingerprint, simplified trunk collision, impostor/instancing/wind path и renderer mapping для remaining persisted flags.

### ED-013 — Voxel layers и rebuild остаются поверх legacy payload

- Категория: Engineering Debt / Performance Debt.
- Приоритет: P2.
- Подтверждение: [voxel-editor.md](voxel-editor.md), [architecture.md](architecture.md), [verification-report.md](verification-report.md).
- Описание: native Qt tools, selection clipboard, procedural generators, material-index layers и delta undo работают поверх одного существующего sparse `VoxelGroup`. Два слоя не могут независимо занять одну координату; hidden layer остаётся в physics payload; любая команда пересобирает весь mesh/physics object. Expected revision пока хранит только последний hash на UID, поэтому запоздалый echo более ранней из нескольких быстрых локальных команд может безопасно, но излишне очистить undo history. Marching Cubes и panel import/export ещё не реализованы.
- Причина: интеграция намеренно не меняла shared/network/disk schema и сохранила совместимость с существующим server/clients.
- Последствия: редактор функционален для bounded объектов, но крупные models дают дорогой rebuild, layer semantics ограничены, а renderer smoothing/export нельзя объявлять готовыми.
- Текущее смягчение: operation/dimension caps, atomic commands, immediate revision guard перед undo/redo, parcel-AABB rollback для redo, per-UID delta history, Greedy meshing, transparency-aware cache key и clean-room regression smoke в Release/RelWithDebInfo.
- Предлагаемое решение: очередь подтверждаемых revision/operation IDs, затем отдельный versioned compressed layer/chunk payload с backward-compatible flattening, dirty chunk meshing, selection overlay/manipulator, Marching Cubes и format adapters.

## Когда обновлять

Обновлять register при появлении нового подтверждённого долга, изменении приоритета, исправлении записи или переносе в отдельный ADR/план. Не использовать этот файл как общий backlog идей.
