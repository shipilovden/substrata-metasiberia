# Verification Report: Documentation and Knowledge System

Назначение: постоянный итоговый отчёт крупных documentation/knowledge tasks. Не является текущим architecture source; для current facts использовать профильные документы `docs/codex`.

Разделы до `## Phase 5` сохраняют итог Phase 2/3 migration и post-migration hardening.

Дата: 2026-07-10

Baseline: `master` / `2ef62fd6`, синхронно с `origin/master` на старте

Scope: `docs/codex`, canonical `MCOS.md`, root/local `AGENTS.md`

Запрещённые действия: build, tests, client/server run, deploy, commit, push, production — не выполнялись

## Итог

Создана единая цепочка знаний:

```text
рабочий код
  -> docs/codex (факты и карты)
  -> MCOS (инженерный стандарт)
  -> AGENTS.md (компактные loaders)
```

Scientific Object Editor документирован как официальный WIP с текущей Qt-based реализацией: реальная generic-object модель отделена от нормативной долгосрочной цели MCOS и от отсутствующих adapters/AI/simulation capabilities.

## Обновлённые документы

| Документ | Основные изменения |
| --- | --- |
| `MCOS.md` | canonical Markdown engineering constitution; source hierarchy, normative/current boundary, исправленные Web/Server/Public/Scientific роли, удалённые дубли |
| `system-overview.md` | главный product/system overview |
| `project-index.md` | основной навигатор task -> doc -> owner |
| `project-map.md` | полная карта требуемых подсистем и paths |
| `architecture.md` | logical/physical/process architecture и Scientific WIP |
| `component-relations.md` | logical/build/deployment dependencies |
| `data-map.md` | existing domain/runtime/local/generated/scientific data |
| `current-state.md` | real baseline/dirty/WIP/partial/plan status |
| `decisions.md` | 14 ADR records |
| `development-rules.md` | Development Intelligence + workflow + Scientific rules |
| `search-guide.md` | Token Intelligence routing |
| `token-policy.md` | research levels/minimal reading/reuse/economy |
| `documentation-index.md` | единый portal |
| `audit-report.md` | актуальный evidence/gaps report |
| `build-and-test.md` | purpose/freeze/Scientific classification; commands unchanged |
| `documentation-changelog.md` | единый changelog format |
| `docs/XR_LOCAL_AVATAR_VISIBILITY_FIX_2026-03-27.md` | 14 stale absolute local line links заменены portable relative links |

## Новые документы

- `scientific-object-editor.md` — canonical technical WIP reference.
- `verification-report.md` — этот итоговый отчёт.

`инструкция.txt` не сохранён как второй источник: его содержимое перенесено в canonical `MCOS.md`, а все internal references обновлены.

## Добавленные разделы и знания

- logical vs physical/build/deployment dependencies;
- exact Scientific owners, marker, lifecycle и schema field groups;
- server-opaque generic WorldObject persistence model;
- `WorldObject::MAX_CONTENT_SIZE = 10 000` как correctness boundary;
- temporary molecule OBJ/MTL -> checksum `.bmesh` conversion path и необходимость runtime/reconnect validation;
- QSettings AI key path и отсутствие security review;
- explicit implementation vs mock/placeholder matrix;
- WIP exit criteria и manual test matrix;
- четыре уровня исследования и stop rules;
- canonical knowledge ownership/update triggers;
- ADR для knowledge hierarchy и Scientific provisional envelope.

## Удалённые/сокращённые разделы

- дублирующий MCOS Appendix I, повторявший Workflow Appendix E;
- два одинаковых финальных «замечания архитектора»;
- prose о «следующей главе» внутри завершённого MCOS;
- ложная самостоятельность Web Client;
- standalone assumptions для embedded HTTP/WS/Admin;
- повтор архитектурной книги в root AGENTS;
- component facts, команды и production detail из local loaders, когда они уже canonical в docs.

Historical/release документы и code files не удалялись.

## Устранённые противоречия

1. MCOS vs code: Web Client теперь shared `gui_client` codebase.
2. Logical vs physical: web/admin разделены логически, но остаются в `server` process.
3. Public vs Server Website: external `metasiberia.com` не приписан repo assets.
4. MCOS scientific goals vs code: goals normative; current implementation WIP и ограничена.
5. Scientific schema vs network: зафиксирован общий 10 KB limit.
6. UI vocabulary vs implementation: databases/providers/formats не называются working adapters.
7. AGENTS vs knowledge base: loaders больше не дублируют полный audit.
8. Phase 1 status vs owner instruction: Scientific Object официально WIP, не случайный untracked artifact.

## Перенос знаний

- Product surfaces -> `system-overview.md`.
- Task routing -> `project-index.md`/`search-guide.md`.
- Physical owners -> `project-map.md`.
- Contracts/flows -> `architecture.md`/`component-relations.md`.
- Scientific details -> `scientific-object-editor.md`.
- Storage/sensitivity -> `data-map.md`.
- Status/readiness -> `current-state.md`.
- Stable rationale -> `decisions.md`.
- Engineering behavior -> `development-rules.md`/MCOS.
- Session entry -> AGENTS.

## Основные документы после migration

Старт: `AGENTS.md` -> `project-index.md`.

Главный обзор: `system-overview.md`.

Архитектура: `architecture.md` + `component-relations.md`.

Данные: `data-map.md`.

Scientific WIP: `scientific-object-editor.md`.

Статус: `current-state.md`.

Инженерный стандарт: `MCOS.md` + `development-rules.md`.

Все документы: `documentation-index.md`.

## AGENTS

Переписаны:

- root `AGENTS.md`;
- `gui_client/AGENTS.md`;
- `server/AGENTS.md`;
- `webserver/AGENTS.md`;
- `shared/AGENTS.md`;
- `webclient/AGENTS.md`;
- `scripts/AGENTS.md`;
- `docs/AGENTS.md`.

Удалено: 0. Создано в Phase 3: 0. Existing local loaders сохранены, потому что каждый имеет уникальные subsystem rules. Отдельный Scientific AGENTS не создан: WIP расположен непосредственно в `gui_client/`.

Размеры после повторной проверки:

| Loader | Строк |
| --- | ---: |
| root `AGENTS.md` | 94 |
| `gui_client/AGENTS.md` | 38 |
| `server/AGENTS.md` | 25 |
| `webserver/AGENTS.md` | 22 |
| `shared/AGENTS.md` | 21 |
| `webclient/AGENTS.md` | 19 |
| `docs/AGENTS.md` | 19 |
| `scripts/AGENTS.md` | 18 |

Автоматическая проверка не нашла одинаковых содержательных строк длиной от 25 символов между разными loaders. Дополнительное сокращение не требуется; root остаётся около рекомендуемого MCOS минимума, locals читаются за несколько секунд.

## Проверка сохранности MCOS

- Исходный сохранённый документ содержал 1710 строк.
- До Markdown-переноса он имел 1521 строку после удаления дублей и исправления противоречий; net reduction составил 189 строк.
- Удалялись дублирующий task-template appendix, transition prose и повторный финальный comment; долгосрочные приложения A–I и Epilogue сохранены.
- Сохранены Strategic Goal, Scientific platform principles, Coding Standards, Workflow Templates, Documentation Maintenance, Large Repository Strategy, Token Economy и Continuous Improvement.
- Текущий `MCOS.md` содержит 1552 строки после добавления Markdown title/headings, TOC и stable appendix anchors.
- Post-migration hardening не переписывал философию MCOS; изменения были форматными и навигационными.

## Статическая проверка

Проверяется перед завершением:

- local Markdown links и target files;
- referenced headings/anchors;
- stale terminology and deleted components;
- duplicate MCOS appendices/notes;
- AGENTS hierarchy and duplicated blocks;
- `git diff --check` и final status/readback.

- 0 broken local Markdown targets в `docs/codex` и AGENTS.
- 0 broken local heading anchors в `docs/codex` и AGENTS.
- Все files `docs/codex` перечислены в documentation portal.
- 53 tracked first-party Markdown documents проверены: 0 broken local targets и 0 absolute local Markdown links после исправления XR history.
- MCOS содержит одну последовательность Appendices A–I, без transition prose и duplicated final note.
- Восемь AGENTS имеют уникальные области; root остаётся компактным loader.
- Explicit trailing-whitespace scan и `git diff --check` прошли без замечаний.

Build/test/runtime намеренно не входят в verification этой задачи.

## Phase 4: аудит структуры документации

Режим: **proposal-only**. Результаты ниже не применялись автоматически.

### Что уже оптимально

- В `docs/codex` нет orphan documents: каждый имеет хотя бы одну входящую ссылку.
- `project-index.md` напрямую маршрутизирует к 16 профильным документам.
- `documentation-index.md` является portal и связывает все 18 canonical files.
- `system-overview`, `architecture`, `project-map` и `project-index` имеют разные вопросы; объединять их не рекомендуется.
- `search-guide` описывает практический поиск, `token-policy` — глубину и стоимость исследования; точного содержательного дублирования между ними не найдено.
- Среди длинных строк canonical docs найден только один ожидаемый повтор — ссылка на Scientific canonical document.
- Все восемь AGENTS имеют уникальную область; лишних loaders не обнаружено.

### Предложения Phase 4 и статус после Phase 5

1. **Выполнено в Phase 5:** создать `glossary.md` для Scientific Object, marker, payload, realtime, embedded webserver, WorldObject, pipeline, persistence и других cross-component терминов.
2. **Выполнено в Phase 5:** в `project-index.md` направить Scientific task непосредственно на `scientific-object-editor.md`, а `data-map.md` оставить дополнением.
3. **Оставлено как Documentation Debt:** создать `docs/codex/adr/`; оставить `decisions.md` индексом и постепенно перенести 14 ADR в отдельные immutable records.
4. **Оставлено как Documentation Debt:** явно классифицировать 11 first-party documents, не названных по имени в portal: admin backlog, release notes `0.0.13`–`0.0.20` и два temporary Wiki capability plans.
5. **Оставлено как Documentation Debt:** после стабилизации migration переместить `audit-report.md` и `verification-report.md` в dated audits/history area либо пометить их snapshot documents.
6. **Открыто:** рассмотреть ADR naming/numbering policy до создания структуры; не создавать пустые directories заранее.

### Что не рекомендуется

- Не объединять `project-index` с `documentation-index`: первый маршрутизирует задачи, второй каталогизирует документы.
- Не объединять `system-overview` с `architecture`: обзор и детальная boundary model имеют разную стоимость чтения.
- Не удалять local AGENTS: каждый содержит subsystem-specific invariants.
- Не дробить MCOS автоматически: TOC и stable anchors уже позволяют читать нужное приложение; split потребует отдельного решения о canonical constitution.

## Рекомендуется проверить владельцу вручную

1. Смысловую полноту MCOS после удаления повторного task-template appendix; автоматическая проверка подтверждает сохранение всех долгосрочных приложений, но owner review остаётся окончательным.
2. Intended roadmap Scientific Object и перечень реально приоритетных disciplines/adapters.
3. После отдельного разрешения — Qt compile/manual Scientific flow.
4. Payload near/over 10 KB, malformed JSON, reconnect и old-client behavior.
5. Generated molecule model после второго клиента/restart; content-addressed upload.
6. API key absence в object/network/log и выбор approved credential store.
7. Реальные online/AI/file buttons должны быть disabled/labelled mock до implementation.
8. `features.htmlfrag`, server seed staging и external Public Website/TheRift ownership.
9. Live production status только перед отдельной production задачей.

## Непроверено

- compile/test/runtime correctness dirty Scientific/Particle code;
- live production/deployed assets;
- external repositories/services;
- security sufficiency auth/session/QSettings;
- optional root-disabled targets.

## Phase 5: Knowledge System Expansion

Дата: 2026-07-10

Scope: `docs/codex` as engineering knowledge base; no code/build/runtime/deploy actions.

### Выполнено

- Проанализирована существующая структура `docs/codex`: portal, navigator, MCOS, ADR, reports, current state, Scientific WIP.
- Создан [glossary.md](glossary.md) как окупаемый navigation/terminology document для cross-component терминов.
- Создан [engineering-debt.md](engineering-debt.md) как постоянный debt register с категориями, приоритетами и подтверждёнными entries.
- [documentation-index.md](documentation-index.md) получил уровни A-F и новые glossary/debt routes.
- [project-index.md](project-index.md) получил маршруты для терминов и debt; Scientific route теперь ведёт прямо в canonical [scientific-object-editor.md](scientific-object-editor.md).
- [MCOS.md](MCOS.md), [development-rules.md](development-rules.md), [search-guide.md](search-guide.md) и [token-policy.md](token-policy.md) обновлены под Phase 5 principles: payoff rule, architectural responsibility, debt capture and minimal document creation.
- [documentation-changelog.md](documentation-changelog.md) получил Phase 5 entry.

### Не выполнено

- `docs/codex/adr/` не создан.
- `decisions.md` не дробился на 14 отдельных ADR files.
- `audit-report.md` и `verification-report.md` не переносились в history subtree.
- `glossary.md` не расширялся до полного словаря всех классов/functions.

### Почему

- ADR split сейчас создал бы много новых файлов с тем же содержанием и нарушил бы Phase 5 rule "document must pay for itself". Решение зафиксировано как Documentation Debt.
- Исторические отчёты уже классифицированы portal/status rules; перемещение потребует отдельной policy, чтобы не сломать ссылки.
- Глоссарий ограничен cross-component терминами, потому что класс/функция лучше ищется через source и [search-guide.md](search-guide.md).

### Оставшиеся риски

- Приоритеты debt entries требуют owner review.
- Glossary может потребовать пополнения после нескольких реальных задач, особенно по Chat, Map, Resources и Bots.
- ADR directory станет полезнее при следующем крупном архитектурном решении или росте числа ADR.
- Никакая build/test/runtime проверка не выполнялась по запрету задачи.

### Рекомендуется владельцу

1. Проверить приоритеты в [engineering-debt.md](engineering-debt.md).
2. Решить, когда переносить ADR в `docs/codex/adr/`: сейчас, при следующем архитектурном решении или после порога роста.
3. После 2-3 будущих задач проверить, помогает ли [glossary.md](glossary.md) реально сокращать повторное исследование.
4. Отдельной cleanup-задачей решить, нужен ли `docs/codex/history/` для audit/verification snapshots.

## Scientific Object Editor: base logic correction

Дата: 2026-07-10

Scope: Scientific Object Editor source/provider behavior, schema/status/provenance, visualization update fingerprint, basic physics/material controls and related docs. Build/test/client/server/deploy/commit/push не выполнялись.

### Выполнено

- Устранён старый mock-success path: `previewMockOnlineResult`, `loadMockOnlineResult`, `setOnlineMockResult` заменены на status-driven `previewScientificSourceResult`, `loadScientificSourceResult`, `setScientificSourceResult`.
- Пустой query больше не заменяется на `Caffeine`.
- Unsupported provider/type теперь показывает `Unsupported` и не меняет scientific tables.
- Built-in samples отделены от external providers: реализованы только локальные `Caffeine` и `Water` с provenance `built-in:*`.
- Schema v1 расширена полями `schema_version`, `load_status`, `data_origin`, `provenance_*`, label/animation runtime status, physics descriptors, glow/outline/wireframe descriptors.
- Unknown root JSON fields сохраняются при read/write.
- `ScientificObjectEditor::toObject` проверяет serialized payload against `WorldObject::MAX_CONTENT_SIZE`.
- Molecule generated-model fingerprint учитывает visual controls, влияющие на mesh.
- New Scientific Object default physics стал non-solid/non-collidable через existing WorldObject flags.
- Добавлены UI controls/tooltips для physics, labels, glow, wireframe и source status.
- Обновлены [scientific-object-editor.md](scientific-object-editor.md), [data-map.md](data-map.md), [current-state.md](current-state.md), [component-relations.md](component-relations.md), [search-guide.md](search-guide.md), [engineering-debt.md](engineering-debt.md).

### Не выполнено

- Реальные PubChem/RCSB/AlphaFold/NCBI/Materials/EMDB/etc. adapters не реализованы.
- File parsers для PDB/MOL/SDF/XYZ/CIF/PLY/LAS/DICOM/NIfTI/etc. не реализованы.
- Runtime labels через child `ObjectType_Text` не реализованы.
- Animation tick/update integration не реализована.
- Build/runtime/manual Qt flow не проверялись по запрету задачи.

### Почему

- Старый код выдавал fixed molecule table как “PubChem adapter placeholder”, что нарушало научную достоверность.
- Реальные adapters требуют сетевой/API/import architecture и verification; в этой задаче безопаснее показать unsupported, чем создавать fake integrations.
- Labels/animation требуют отдельного lifecycle/update design, чтобы не ломать generic WorldObject/server flow.

### Оставшиеся риски

- Все code changes требуют owner-approved build/runtime verification.
- Payload guard and generated-model fingerprint are statically added but not runtime-tested.
- Built-in Caffeine sample remains demo data and must not be cited as external scientific source.
- Provider list still intentionally contains planned entries; UI must remain clear about unsupported status.

### Рекомендуется владельцу

1. Разрешить отдельную local Qt build/manual test task.
2. Проверить flow: molecule type + `Caffeine`, molecule type + `Water`, molecule type + `Nicotine`, protein/DNA/RNA provider choice, empty query.
3. Проверить payload > 10 KB, malformed JSON, unknown-field round trip.
4. Выбрать первый real adapter/importer for implementation; не начинать сразу со всех databases.
5. Отдельно спроектировать Scientific labels через existing text system и animation runtime adapter.

## Scientific Object Editor build verification

Дата: 2026-07-10

Scope: owner-approved Windows/Qt build after Scientific Object Editor static changes. Tests, client runtime, server runtime, deploy, commit and push were not performed.

### Команда

```powershell
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1
```

### Результат

- Итоговый статус: **CONFIRMED**.
- Exit code: 0.
- Попыток сборки: 1.
- Build dir: `C:\programming\substrata_build_qt`.
- Output root: `C:\programming\substrata_output_qt`.
- Manifest: `C:\programming\substrata_output_qt\build_manifest.json`.
- Target: `gui_client`.
- Configs: `Release`, `RelWithDebInfo`.
- Generator: Visual Studio 17 2022 x64.
- Qt: 5.15.16 at `C:/programming/Qt/5.15.16-vs2022-64`.
- CEF: OFF.
- XR: Auto resolved to `XR_SUPPORT=ON` with `C:\programming\OpenXR-SDK-1.1.57\install`.

Confirmed artifacts:

```text
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\Release\gui_client.exe
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe
```

### Исправления после первой попытки

- Ошибки компиляции/линковки, вызванные текущими Scientific Object Editor changes: не обнаружены.
- Исходные файлы после первой попытки сборки: не изменялись.
- Обновлены только постоянные документы с результатом подтверждения build.

### Замечания

- Runtime copy printed a warning about missing `C:/programming/SDL/sdl_2.30.9_build/Release/SDL2.dll`; wrapper treats this as non-fatal for the Qt build, and required artifact validation completed successfully.
- Runtime/UI проверка Scientific Object Editor не выполнялась владельцем и не должна считаться подтверждённой этим build.

## Scientific data provider reference

Дата: 2026-07-10

Scope: knowledge-system expansion for future Scientific Object data loading. Code, build, tests, client runtime, server runtime, deploy, commit and push were not performed.

### Выполнено

- Создан [scientific-data-providers.md](scientific-data-providers.md) как canonical reference for scientific sources/API/formats/provider architecture/provenance.
- Зафиксирована целевая registry-based схема `ScientificProviderRegistry -> ScientificDataProvider -> concrete providers`.
- Описан lifecycle: search -> metadata -> fetch -> parse -> model -> cache -> visualizer -> Scientific Object.
- Описано, что хранить в marker/JSON, external resources, local cache и потенциальном server cache.
- Добавлена provider matrix for PubChem, RCSB PDB, ChEBI, ChemSpider, AlphaFold DB, UniProt, NCBI, COD, Materials Project, OQMD, EMDB and LocalFileProvider.
- Навигация обновлена в [documentation-index.md](documentation-index.md), [project-index.md](project-index.md), [search-guide.md](search-guide.md), [scientific-object-editor.md](scientific-object-editor.md), [data-map.md](data-map.md) и [documentation-changelog.md](documentation-changelog.md).

### Не выполнено

- Реальные provider classes/parsers/cache не реализованы.
- Runtime/API calls не выполнялись.
- API keys/access for Materials Project/ChemSpider не проверялись владельцем.

### Почему

Текущая задача была документационной: отделить долговременную provider/API specification от рабочих prompts и current WIP implementation status.

### Оставшиеся риски

- Official API docs and limits can change; provider implementation tasks must re-check the exact official source before coding.
- Некоторые provider docs are JS/Swagger-driven, поэтому future implementation must validate endpoint details directly.
- Лицензии и redistribution terms must be checked before server-side caching or public sharing.

### Рекомендуется владельцу

1. Выбрать первый real adapter: local file import, PubChem or RCSB.
2. Для Materials Project/ChemSpider заранее решить credential policy.
3. Не начинать одновременную реализацию всех providers; идти по одному provider с fixtures/provenance/error handling.

## Scientific Object Editor Phase 1: PubChem molecules

Дата: 2026-07-11

Scope: implement the first molecule-provider vertical slice in the Qt Scientific Object Editor. Tests, client runtime/manual UI flow, server runtime, deploy, commit and push were not performed.

### Выполнено

- PubChem molecule provider path added in `gui_client/ScientificObjectEditor.cpp`.
- Search uses PubChem PUG REST endpoint patterns for CID/name/SMILES/InChI/InChIKey/formula heuristics.
- Result list requires explicit CID selection before loading; first result is not auto-loaded.
- Load path fetches property JSON, SDF structure, PNG image and optional synonyms/PUG View title.
- SDF parser creates atom/bond tables used by the existing molecule OBJ/MTL visualizer path.
- Local PubChem HTTP cache stores provider responses by hashed request key.
- Retry/backoff added for HTTP 429/503 with no unbounded retry loop.
- Schema adds provider/parser/cache/image/checksum/conformer fields while preserving unknown root JSON fields.
- PubChem provenance stores provider, CID, source URL, loaded time, format, parser/provider/cache versions and SHA-256 checksum.
- Built-in samples remain separate from PubChem and are not used as PubChem fallback.
- Other providers/types remain planned/unsupported and must not mutate molecule data.
- Windows/Qt build completed successfully.

### Build

Command:

```powershell
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1
```

Result:

- Status: **CONFIRMED** for compile/link/runtime-copy artifact validation.
- Attempts: 1.
- Manifest: `C:\programming\substrata_output_qt\build_manifest.json`.
- Manifest `success`: `true`.
- Configs: `Release`, `RelWithDebInfo`.
- Confirmed artifacts:

```text
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\Release\gui_client.exe
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe
```

Non-fatal warning: wrapper still reports missing `C:/programming/SDL/sdl_2.30.9_build/Release/SDL2.dll`; both Qt configs completed successfully and manifest reports success.

### Не выполнено

- Owner manual runtime verification of PubChem search/load was not performed.
- Interactive scene labels through child `WorldObject::ObjectType_Text` are not implemented.
- Scene legend overlay is not implemented; legend summary is generated in `value_table`.
- Interactive atom selection, distance/angle/torsion tools are not implemented.
- Animation tick/update integration is not implemented.
- Local file import parsers are not claimed as implemented.
- ChEBI/ChemSpider/RCSB/AlphaFold/NCBI/COD/Materials/OQMD/EMDB providers are not implemented.

### Почему

The safe vertical slice was PubChem molecule search/load/cache/provenance plus existing molecule visualization. Labels, interactive measurements and animation need separate runtime lifecycle hooks so they are left as explicit Engineering Debt instead of fake runtime features.

### Риски

- PubChem online behavior still needs owner runtime testing with live network conditions.
- PubChem endpoint availability/rate limiting can change; UI must show errors instead of fallback objects.
- `WorldObject::content` remains bounded by 10 KB; larger molecules may need resource-backed scientific data storage.

### Рекомендуется владельцу

1. Manually test `nicotine`, `ethanol`, `glucose`, invalid query and offline behavior.
2. Verify explicit CID selection, 2D image preview, 3D/2D conformer status, save/reconnect and cache status.
3. Open separate tasks for text labels, scene legend, interactive atom selection/measurements and visual animation runtime.

## Scientific Object Editor PubChem HTTPS runtime fix

Дата: 2026-07-11

Scope: fix runtime PubChem HTTPS failure `Protocol "https" is unknown` after Phase 1 molecule implementation. Server runtime, deploy, production, commit and push were not performed.

### Причина

- The new PubChem provider initially used Qt `QNetworkAccessManager` for `https://pubchem.ncbi.nlm.nih.gov`.
- The local Qt Network module is built with SSL disabled. Evidence: `Qt5NetworkConfig.cmake` lists `ssl`, `schannel` and `openssl` as disabled features.
- Because SSL is disabled at Qt build level, `QNetworkAccessManager` cannot handle `https://` and reports `Protocol "https" is unknown`.
- This was not a PubChem query/data issue and not caused by missing OpenSSL DLLs beside `gui_client.exe`.

### Выполнено

- PubChem network transport was moved from QtNetwork HTTPS to Windows WinHTTP/SChannel inside `gui_client/ScientificObjectEditor.cpp`.
- HTTPS was preserved; no HTTP downgrade was added.
- TLS certificate errors are not ignored. WinHTTP uses Windows certificate validation by default.
- PubChem URL host is restricted to `pubchem.ncbi.nlm.nih.gov` for this transport path.
- Added diagnostic status/log text identifying WinHTTP/SChannel and the disabled QtNetwork SSL condition.
- Added narrow headless client smoke mode: `gui_client.exe --scientific_pubchem_smoke <report.json>`.
- Added `winhttp` link dependency for `gui_client`.

### Build

Command:

```powershell
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1
```

Result:

- Status: **CONFIRMED**.
- Final manifest: `C:\programming\substrata_output_qt\build_manifest.json`.
- Manifest `success`: `true`.
- Configs: `Release`, `RelWithDebInfo`.
- Confirmed artifacts:

```text
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\Release\gui_client.exe
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe
```

Build attempts during this task:

1. Failed CMake configure due to mixed `target_link_libraries` signatures after adding `winhttp`.
2. Failed compile because `QSslSocket` is unavailable when Qt SSL is disabled.
3. Succeeded after moving diagnostics away from `QSslSocket`.
4. Succeeded after adding the final headless PubChem smoke command-line path.

### Runtime smoke

Command:

```powershell
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe --scientific_pubchem_smoke <report.json>
```

Result:

- Exit code: 0.
- Status: **CONFIRMED for headless provider smoke**.
- `water`: search HTTP 200, selected CID 962, properties HTTP 200, SDF 3D HTTP 200, atom count 3, bond count 2, PNG HTTP 200.
- `nicotine`: search HTTP 200, selected CID 89594, properties HTTP 200, SDF 3D HTTP 200, atom count 26, bond count 27, PNG HTTP 200.
- Invalid query: HTTP 404/no CID; no built-in sample fallback.
- Report path used during verification: `C:\Users\densh\AppData\Local\Temp\metasiberia_pubchem_smoke_20260711_024334.json`.

### Не выполнено

- Manual editor UI flow was not clicked through by owner after the fix.
- Server-confirm/create-object/reconnect/resource-upload flow was not verified.
- Async cancel UX, stale-response rejection and provider worker lifecycle remain Engineering Debt.

### Рекомендуется владельцу

1. Manually open the Scientific Object Editor and run PubChem `water` and `nicotine`.
2. Confirm the result list appears, CID selection loads metadata/SDF/PNG and source remains `PubChem`.
3. Save object, reconnect/reopen and verify generated model/resource persistence.
4. Test slow/offline network behavior before considering PubChem UI fully production-ready.

## Scientific Object Editor PubChem selected-result application fix

Дата: 2026-07-11

Scope: fix runtime defect where PubChem search returned CID 962 for `water`, but pressing `Загрузить в объект` did not apply the selected result to the Scientific Object. Deploy, production, commit and push were not performed.

### Причина

- `setScientificSourceResult()` cleared `online_results_list` at the start of both preview and load actions.
- When the user pressed `Загрузить в объект`, the previously selected PubChem item was deleted before the handler read `currentItem()`.
- The load path then performed search again and intentionally returned with the old message `Results available: select a PubChem CID and click "Загрузить в объект" again`.
- Additionally, `loadPubChemCID()` updated widgets while normal object-change signals were active, which could emit partial object updates before metadata/SDF/PNG/provenance fields were complete.

### Выполнено

- PubChem load no longer clears the result list before reading selected CID.
- First/single result is selected visibly after search.
- Result selection styling was improved for the dark Qt theme.
- Load button state now reflects the workflow: disabled before query/result, `Загрузить CID ... в объект` after selection, `Загрузка CID ...` during load, `Обновить объект` after success.
- Query/result matching guard prevents loading a stale selected CID after the search text changes.
- PubChem UI field updates are applied atomically under `syncing=true`, then a single final `objectChanged()` is emitted.
- `source_url`, `online_result_id` and provenance/status fields are updated so `source=PubChem` survives `toObject()`.
- Added `gui_client.exe --scientific_pubchem_apply_smoke <report.json>` for QWidget slot-flow verification.

### Build

Command:

```powershell
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1
```

Result:

- Status: **CONFIRMED**.
- Exit code: 0.
- Attempts during this task: 1.
- Confirmed artifacts:

```text
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\Release\gui_client.exe
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe
```

Non-fatal wrapper warning about missing SDL2.dll remained unchanged.

### Runtime verification

Provider network smoke:

- Command: `gui_client.exe --scientific_pubchem_smoke <report.json>`.
- Exit code: 0.
- `water`: live HTTPS, CID 962, formula H2O, SDF 3D, 3 atoms, 2 bonds, PNG available.
- `nicotine`: live HTTPS, CID 89594, formula C10H14N2, SDF 3D, 26 atoms, 27 bonds, PNG available.
- Invalid query: HTTP 404/no CID/no built-in fallback.
- Report path: `C:\Users\densh\AppData\Local\Temp\metasiberia_pubchem_network_smoke_20260711_033314.json`.

Object-application smoke:

- Command: `gui_client.exe --scientific_pubchem_apply_smoke <report.json>`.
- Exit code: 0.
- `water`: selected CID 962 visible, button `Загрузить CID 962 в объект`, `source=PubChem`, `data_origin=provider`, 3 atoms, 2 bonds, PNG cache exists, local molecule OBJ assigned, `MODEL_URL_CHANGED` and `CONTENT_CHANGED` set.
- `nicotine`: selected CID 89594 visible, button `Загрузить CID 89594 в объект`, `source=PubChem`, `data_origin=provider`, 26 atoms, 27 bonds, PNG cache exists, local molecule OBJ assigned, `MODEL_URL_CHANGED` and `CONTENT_CHANGED` set.
- Invalid query: object source stayed `manual`; no fallback sample.
- Report path: `C:\Users\densh\AppData\Local\Temp\metasiberia_pubchem_apply_smoke_20260711_033251.json`.

### Не выполнено

- Full interactive GUI click-through against a live server was not automated.
- Server confirmation, production world persistence, `.bmesh` upload through `GUIClient::objectEdited()`, reconnect and second-client visibility were not verified automatically.
- Async cancellation/provider worker lifecycle remains Engineering Debt.

### Рекомендуется владельцу

1. In the real editor UI, search `water`, confirm CID 962 is visibly selected and click `Загрузить CID 962 в объект` once.
2. Confirm PNG preview appears and the in-world model changes from placeholder cube to water molecule.
3. Confirm title/source changes to PubChem and status becomes Ready/Loaded from cache.
4. Repeat with `nicotine` and then reconnect/reopen the object to verify server/resource persistence.

## Scientific Object Editor Phase 1.2: interactive molecule information layer

Проверено 2026-07-11 без deploy/server/production/commit/push.

### Выполнено

- Added native `MoleculeViewportWidget` and selected-world-object atom/bond ray intersection path.
- Added five selection states, atom/bond/molecule context actions, seven label modes, CPK legend, selected-object 3D world-space label/highlight overlays and compact persisted selection/measurement state.
- Added distance/angle/torsion plus center of mass, bounding dimensions, maximum extent and molecular mass.
- Added Russian-labelled molecule card tabs/content for the active PubChem summary path, immediate REST Summary/Structure/Properties, lazy cached PUG View sections, PubChem PNG source preview on CID application and zoomable image viewer.
- Added exact-first Russian resolver for six required aliases without translating chemical identifiers.
- Added distinct provider/computed/user classification fields, favorites, recents/search history and native periodic table modules with 118 records.
- Rotation/Spin works in the native molecule viewport; all solver-backed simulation controls are disabled and marked backend unavailable.

### Build and smoke

- `C:\programming\qt_build.ps1`: exit 0; Release and RelWithDebInfo success; XR Auto ON; manifest `success=true` at 2026-07-11 11:41 local.
- Follow-up narrow `RelWithDebInfo gui_client` rebuild after world-space overlay/image/localisation fix: exit 0.
- `--scientific_pubchem_apply_smoke`: exit 0 for Water/Nicotine, PubChem PNG preview/cache and local OBJ; latest marker sizes 7,616 bytes and 9,098 bytes.
- `--scientific_molecule_info_smoke`: exit 0; six Russian CIDs, seven rendered label modes, selection states, three measurements, PUG View Classification and 118 elements.
- `QT_QPA_PLATFORM=offscreen` is not valid for the current Windows runtime output because the offscreen platform plugin is absent; smokes were run with the normal Qt platform.

### Не выполнено

- Owner manual UI review, server confirmation/reconnect and second-client persistence.
- SDL/Web parity and persistent `ObjectType_Text` child labels. Per-atom label/highlight overlays exist only as native selected-object editor GL overlays, not server-persisted scene entities.
- Global indexed catalog query across multiple Scientific Objects.
- Molecular dynamics, CFD, orbitals, trajectory/vibration/time-series or other solvers.

## Scientific Object Editor crash follow-up: molecule replacement with existing measurements

Проверено 2026-07-11 без deploy/server/production/commit/push.

### Причина падения

Crash dump `gui_client.exe.25928.dmp` показал Qt assert/security fast fail в `QVector<MoleculeViewportWidget::Atom>::operator[]` из `MoleculeViewportWidget::measurementsJson()`. При загрузке новой молекулы в уже открытый объект `setMolecule()` перестраивал atoms/bonds, но оставлял старые distance/angle/torsion measurement records. Затем `stateChanged` сериализовал measurements и обращался к устаревшим atom indices.

### Исправлено

- `MoleculeViewportWidget::setMolecule()` теперь очищает `measurements` и `measurement_mode` при реальной замене структуры.
- `MoleculeViewportWidget::measurementsJson()` теперь пропускает invalid/empty measurement records вместо обращения к несуществующему atom index.
- `--scientific_molecule_info_smoke` получил regression check `stale_measurements_cleared_after_molecule_replace=true`.

### Проверка

- Narrow rebuild: `cmake --build C:\programming\substrata_build_qt --config RelWithDebInfo --target gui_client -j 8`, exit 0.
- `--scientific_molecule_info_smoke`: exit 0, report `C:\Users\densh\AppData\Local\Temp\metasiberia_molecule_info_after_measurement_crash_fix.json`, status `ok`.
- `--scientific_pubchem_apply_smoke`: exit 0, report `C:\Users\densh\AppData\Local\Temp\metasiberia_pubchem_apply_after_measurement_crash_fix.json`, status `ok`.
- Canonical `C:\programming\qt_build.ps1`: exit 0; Release and RelWithDebInfo success; XR Auto ON.
- Post-canonical `--scientific_molecule_info_smoke`: exit 0, report `C:\Users\densh\AppData\Local\Temp\metasiberia_molecule_info_after_canonical_crash_fix.json`, status `ok`, `stale_measurements_cleared_after_molecule_replace=true`.
- Post-canonical `--scientific_pubchem_apply_smoke`: exit 0, report `C:\Users\densh\AppData\Local\Temp\metasiberia_pubchem_apply_after_canonical_crash_fix.json`, status `ok`; Water/Nicotine image cache and information overlay rendered.
- `git diff --check`: exit 0.

## Scientific Object Editor follow-up: world overlay alignment, Russian selection UI and image preview zoom

Проверено 2026-07-11 без deploy/server/production/commit/push.

### Исправлено

- Selected-atom world overlay no longer uses a large world-axis cube as the primary marker; selected atoms get a local yellow sphere/halo centered on the same atom object-space coordinate as the generated molecule mesh.
- Selected bond gets a yellow world-space cylinder overlay when bond-selection state is active.
- World-space text labels use world-up orientation so they stay upright instead of inheriting camera tilt.
- Selection mode combo and selection status text are Russian-labelled while preserving internal serialized values `atom`, `bond`, `molecule`.
- Source-tab PubChem image preview supports mouse-wheel zoom; full image viewer with pan/fit/reset remains in the Images tab.
- Scientific editor minimum width was reduced from 720 to 360 px in the widget and MainWindow dock setup so the panel can shrink instead of forcing wide overlap.
- Ordinary Rotation/Spin now also advances the selected object's transform through the existing transformChanged path while enabled. This is a visual spin only; solver-based animations remain WIP/unsupported.

### Проверка

- Narrow rebuild: `cmake --build C:\programming\substrata_build_qt --config RelWithDebInfo --target gui_client -j 8`, exit 0.
- Canonical `C:\programming\qt_build.ps1`: exit 0; Release and RelWithDebInfo success; XR Auto ON.
- Post-canonical `--scientific_molecule_info_smoke`: exit 0, report `C:\Users\densh\AppData\Local\Temp\metasiberia_molecule_info_after_canonical_world_overlay_fix.json`, status `ok`.
- Post-canonical `--scientific_pubchem_apply_smoke`: exit 0, report `C:\Users\densh\AppData\Local\Temp\metasiberia_pubchem_apply_after_canonical_world_overlay_fix.json`, status `ok`; Water/Nicotine image and overlay paths reported.
- `git diff --check`: exit 0.
