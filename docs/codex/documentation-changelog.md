# Журнал документации Metasiberia

Назначение: краткий record изменений структуры, роли и достоверности knowledge base. Это не замена Git history и не журнал опечаток.

Формат записи: дата/фаза -> path/группа -> тип -> изменение -> evidence/причина.

## 2026-07-11 — Scientific Object Editor Phase 1.2 information layer

### Follow-up world-space overlay/image/localisation fix

| Область | Изменение | Проверка |
| --- | --- | --- |
| `GUIClient.cpp` | selected Scientific Object now mirrors atom labels and yellow selected-atom highlights into 3D world-space editor GL overlays | narrow `RelWithDebInfo gui_client` build exit 0 |
| `ScientificObjectEditor.*` | molecule card tabs/active PubChem summary path translated to Russian; PubChem PNG source preview loads on CID application; status reports loaded preview | PubChem apply smoke exit 0 |
| `MoleculeViewportWidget.*` | preview label font scale clamped so large UI label scale no longer produces unreadable oversized text | molecule-information smoke exit 0 |
| `docs/codex/build-and-test.md` | recorded that current Windows Qt runtime lacks the offscreen platform plugin; do not force `QT_QPA_PLATFORM=offscreen` for these smokes | cdb stack showed QApplication platform integration fatal before app code |

### Подтверждено

| Область | Изменение | Проверка |
| --- | --- | --- |
| `MoleculeViewportWidget.*` | native molecule projection, labels, CPK legend, atom/bond picking, five selection states, context actions, distance/angle/torsion and derived metrics | `--scientific_molecule_info_smoke` exit 0 |
| `ScientificObjectEditor.*` | scene ray handoff, Russian exact-first resolver, lazy PUG View card sections, lazy image viewer, classification/catalog states, honest animation/simulation status | PubChem apply + information smoke exit 0 |
| `PeriodicTable.*` | 118-element table/list/native 3D property graph and molecule element integration | periodic count 118 in smoke |
| `ScientificImageViewer.*` | fit/zoom/wheel/pan/reset/open/save/source/license | lazy PNG check in apply smoke |
| `ScientificObjectSettings.*` | compact selection/measurements/classification/search/cache fields under existing marker v1 | object apply smoke + payload guard path |
| canonical Qt build | Release + RelWithDebInfo, XR Auto ON, runtime copy | `C:\programming\qt_build.ps1`, manifest success 2026-07-11 11:41 local |

Full manual UI/server/reconnect, SDL/Web parity, deploy, commit and push were not performed.

## 2026-07-11 — Scientific Object Editor PubChem selected-result application fix

### Подтверждено

| Путь | Тип | Изменение | Evidence |
| --- | --- | --- | --- |
| `gui_client/ScientificObjectEditor.*` | UI/state/runtime | PubChem load no longer clears selected result before reading CID; first result is visibly selected; load button applies selected CID in one click; widget updates are atomic before final `objectChanged()` | screenshot defect; `--scientific_pubchem_apply_smoke` exit 0 |
| `gui_client/MainWindow.cpp` | runtime verification | added `--scientific_pubchem_apply_smoke <report.json>` narrow QWidget apply smoke path | apply smoke confirmed `water`/`nicotine` object application |
| `docs/codex/scientific-object-editor.md` | status/runtime | recorded distinction between provider network smoke, QWidget object-application smoke and full GUI/server verification | build + smoke reports |
| `docs/codex/scientific-data-providers.md` | provider status | PubChem status updated from network/parser only to selected-CID object application smoke | apply smoke report |
| `docs/codex/build-and-test.md` | runtime command | added PubChem object-application smoke command and scope boundary | new CLI verification path |
| `docs/codex/current-state.md` | current status | Scientific WIP now records PubChem selected-result application fix | canonical build + apply smoke |
| `docs/codex/engineering-debt.md` | debt register | clarified remaining debt: full GUI/server/reconnect and async/cancel lifecycle, not selected-CID loss | apply smoke fixed CID loss |
| `docs/codex/verification-report.md` | verification | added selected-result application fix section with cause, build, smoke results and manual checks | task completion evidence |

## 2026-07-11 — Scientific Object Editor PubChem HTTPS runtime fix

### Подтверждено

| Путь | Тип | Изменение | Evidence |
| --- | --- | --- | --- |
| `gui_client/ScientificObjectEditor.*` | runtime/network | PubChem transport moved from QtNetwork HTTPS to WinHTTP/SChannel because current QtNetwork has SSL disabled | runtime error `Protocol "https" is unknown`; Qt5Network config disabled `ssl`; smoke exit 0 |
| `gui_client/CMakeLists.txt` | build link | added `winhttp` link dependency for Windows Qt client | successful canonical build |
| `gui_client/MainWindow.cpp` | runtime verification | added `--scientific_pubchem_smoke <report.json>` narrow provider smoke path | smoke report for `water`/`nicotine` |
| `docs/codex/scientific-object-editor.md` | status/runtime | recorded WinHTTP/SChannel transport, headless PubChem smoke and manual UI caveat | build manifest + smoke report |
| `docs/codex/scientific-data-providers.md` | provider status | PubChem status updated to headless runtime-confirmed for `water`/`nicotine` | `gui_client.exe --scientific_pubchem_smoke` exit 0 |
| `docs/codex/build-and-test.md` | build/runtime command | added narrow PubChem smoke command and QtNetwork SSL-disabled note | canonical build + smoke verification |
| `docs/codex/current-state.md` | current status | Scientific WIP now records PubChem HTTPS fix and partial runtime evidence | build manifest success + smoke report |
| `docs/codex/engineering-debt.md` | debt register | added async/cancel PubChem UI lifecycle debt; provider/reconnect debts clarified | manual UI/server/reconnect not verified |
| `docs/codex/verification-report.md` | verification | added PubChem HTTPS runtime fix section with cause, build attempts, smoke results and remaining checks | owner-authorized runtime fix task |

## 2026-07-11 — Scientific Object Editor Phase 1 PubChem molecule slice

### Подтверждено

| Путь | Тип | Изменение | Evidence |
| --- | --- | --- | --- |
| `gui_client/ScientificObjectEditor.*` | Qt WIP implementation | PubChem molecule search/load path, CID result list, SDF parse, PNG cache/preview, local throttling/backoff and readable tab labels | Windows/Qt build success via `C:\programming\qt_build.ps1`; runtime UI flow still owner-check |
| `gui_client/ScientificObjectSettings.*` | marker/schema | provider/parser/cache/image/checksum/conformer fields added to schema v1 with unknown-field preservation | build success |
| `docs/codex/scientific-object-editor.md` | WIP status | Phase 1 Molecules section added; PubChem status moved from planned to implemented WIP with runtime caveat | source change + build manifest success |
| `docs/codex/scientific-data-providers.md` | provider matrix | external capability vs MetaSiberia implementation status matrix added | task requirement |
| `docs/codex/data-map.md` | data map | PubChem cache/provenance fields and provider caveat updated | schema/provider change |
| `docs/codex/current-state.md` | status snapshot | Scientific WIP now includes PubChem Phase 1 build-confirmed path | build manifest success |
| `docs/codex/component-relations.md` | relations | Scientific flow now includes PubChem -> local cache -> parser -> editor | provider data-flow change |
| `docs/codex/search-guide.md` | navigation | added exact symbols for PubChem provider path | future token-saving route |
| `docs/codex/engineering-debt.md` | debt register | updated provider-placeholder debt; labels/animation remain separate runtime debt | implementation scope boundary |
| `docs/codex/verification-report.md` | verification | added Phase 1 PubChem build verification section | owner-approved build |

## 2026-07-10 — Scientific data provider architecture reference

### Добавлено

| Путь | Тип | Изменение | Причина |
| --- | --- | --- | --- |
| `docs/codex/scientific-data-providers.md` | provider architecture reference | canonical reference для scientific providers/API/formats/cache/provenance | убрать повторение API-спецификаций из будущих prompts и предотвратить fake integrations |

### Обновлено

| Путь | Изменение |
| --- | --- |
| `docs/codex/documentation-index.md` | добавлен новый специализированный документ в portal и уровень E |
| `docs/codex/project-index.md` | добавлен route для scientific sources/API/providers |
| `docs/codex/search-guide.md` | добавлен минимальный маршрут Scientific providers/API |
| `docs/codex/scientific-object-editor.md` | добавлена ссылка: implementation status остаётся здесь, API/provider policy вынесена отдельно |
| `docs/codex/data-map.md` | добавлена ссылка на provider/storage/provenance reference |
| `docs/codex/verification-report.md` | добавлен итоговый раздел по созданию provider reference |

## 2026-07-10 — Scientific Object Editor build verification

### Подтверждено

| Путь | Тип | Изменение | Evidence |
| --- | --- | --- | --- |
| `docs/codex/build-and-test.md` | build evidence | `C:\programming\qt_build.ps1` переведён в статус `CONFIRMED` для Windows/Qt `gui_client` build после Scientific Object Editor changes | команда завершилась с exit code 0; `build_manifest.json` success=true |
| `docs/codex/scientific-object-editor.md` | WIP status | добавлен build status `CONFIRMED`; runtime/UI verification оставлен неподтверждённым | Release и RelWithDebInfo artifacts созданы wrapper-ом |
| `docs/codex/verification-report.md` | verification | добавлен раздел `Scientific Object Editor build verification` | owner-approved build task |
| `docs/codex/current-state.md` | status snapshot | Scientific dirty tree теперь имеет подтверждённую Windows/Qt compile проверку, но без runtime/manual UI flow | локальный build 2026-07-10 |

## 2026-07-10 — Phase 5: Knowledge System Expansion

### Добавлено

| Путь | Тип | Изменение | Причина |
| --- | --- | --- | --- |
| `docs/codex/glossary.md` | navigation/terminology | cross-component vocabulary, boundaries and canonical routes | Phase 5 выявил отсутствие единого места для терминов, которые повторяются между docs/code tasks |
| `docs/codex/engineering-debt.md` | debt register | categories, priorities and confirmed debt entries | Engineering Debt должен фиксироваться отдельно от audit/report prose и не исправляться автоматически |

### Переработано

| Путь | Новая роль/изменение |
| --- | --- |
| `documentation-index.md` | добавлены уровни A-F, glossary/debt register и правило окупаемости нового документа |
| `project-index.md` | добавлены маршруты для терминов и debt; Scientific route ведёт прямо в canonical WIP doc |
| `search-guide.md` | glossary/debt включены в Token Intelligence routing |
| `token-policy.md` | терминология и debt capture стали частью reuse strategy |
| `development-rules.md` | добавлены architectural responsibility и debt workflow |
| `architecture.md` | known gaps связаны с debt register |
| `current-state.md` | dirty docs work обновлён до Phase 5 |
| `verification-report.md` | стал постоянным отчётом knowledge/documentation tasks и получил Phase 5 итог |

### Не выполнено намеренно

- `docs/codex/adr/` не создан на Phase 5: без переноса отдельных records это создало бы новую структуру без нового знания. Split остаётся Documentation Debt с низким приоритетом.
- `audit-report.md` и `verification-report.md` не переносились в history subtree: портал уже классифицирует их как snapshot/report, а перемещение потребует отдельной cleanup policy.

## 2026-07-10 — Phase 2: Documentation Migration

### Добавлено

| Путь | Тип | Изменение | Причина |
| --- | --- | --- | --- |
| `docs/codex/scientific-object-editor.md` | canonical WIP doc | owners, marker/schema, lifecycle, implemented/placeholders, 10 KB bound, credentials/resource risks, readiness criteria | Scientific Object признан официальным WIP; требовалась граница code vs MCOS goals |
| `docs/codex/verification-report.md` | verification | пофайловый итог Phase 2/3 и manual follow-up | обязательный финальный отчёт migration |

### Переработано

| Путь | Новая роль/изменение |
| --- | --- |
| `system-overview.md` | главный обзор products/surfaces и Scientific WIP boundary |
| `project-index.md` | основной task navigator вместо component dump |
| `project-map.md` | главная карта Scientific/Native/Web/Public/Server/Admin/Realtime/HTTP-WS/Shared/Pipeline |
| `architecture.md` | logical/physical/process/data architecture + WIP constraints |
| `component-relations.md` | отдельные logical, build и deployment dependency maps |
| `data-map.md` | доменные типы, runtime/local/generated data и Scientific schema model |
| `scientific-object-editor.md` | single canonical home для Scientific WIP details |
| `current-state.md` | baseline vs dirty tree; Scientific официальный WIP, Particle active changes |
| `decisions.md` | ADR format: status/context/decision/why/change/consequences/evidence |
| `development-rules.md` | Development Intelligence workflow + Scientific rules |
| `search-guide.md` | четыре уровня Token Intelligence и task routing |
| `token-policy.md` | research depth, minimal reading, reuse and economy strategies |
| `documentation-index.md` | единый portal core/topic/historical/Wiki/AGENTS |
| `audit-report.md` | актуальный audit snapshot, resolved conflicts и open gaps |
| `build-and-test.md` | добавлены purpose/migration freeze/Scientific row; command blocks сохранены |
| `documentation-changelog.md` | единый датированный формат |
| `docs/XR_LOCAL_AVATAR_VISIBILITY_FIX_2026-03-27.md` | absolute local line links заменены portable relative file links; historical content сохранён |

### MCOS corrections

| Изменение | Причина |
| --- | --- |
| source priority дополнен real architecture/confirmed data/docs chain | соответствие задаче и code-first rule |
| добавлена граница normative standard vs current implementation | MCOS не должен подменять project documentation |
| Web Client исправлен на shared codebase | подтверждено CMake/entry points |
| HTTP/WS/Admin описаны как logical roles одного server deployment | подтверждено server target/process |
| Public Website отмечен external source boundary | source отсутствует в repo |
| Scientific Appendix помечен normative official WIP | цели не равны implemented adapters/platform |
| удалены transition prose, дублирующий Appendix I и двойной architect note | внутреннее дублирование, не-standard content |

### Удалено из актуальных утверждений

- «Web Client — самостоятельная реализация».
- standalone deployment/API assumptions для embedded web/admin.
- утверждение о полностью централизованном/reproducible asset pipeline.
- трактовка UI lists/mocks Scientific Editor как real adapters/AI/import.
- модель Scientific files как неофициальной случайной находки.
- model-specific naming в Token Policy и повторные workflow templates MCOS.

Файлы first-party history/release не удалялись.

## 2026-07-10 — Phase 3: AGENTS Migration

### Переписано

| Путь | Уникальная роль после migration |
| --- | --- |
| `AGENTS.md` | root loader: purpose, hierarchy, research order, core invariants, token/docs rules |
| `gui_client/AGENTS.md` | Qt/SDL/Web/XR/editor + Scientific WIP invariants |
| `server/AGENTS.md` | process/state/concurrency/persistence/production boundaries |
| `webserver/AGENTS.md` | embedded routes/auth/Admin/website asset rules |
| `shared/AGENTS.md` | protocol/model/serialization/10 KB contract |
| `webclient/AGENTS.md` | Emscripten shell/generated/runtime boundary |
| `scripts/AGENTS.md` | side-effect classification and operational safeguards |
| `docs/AGENTS.md` | fact/status/history/portal/Wiki rules |

### Создано/удалено

- Новые AGENTS в Phase 3: нет.
- Удалённые AGENTS: нет; все восемь имеют уникальные регулярно применимые rules.
- Scientific Object не получил отдельный local AGENTS: его files находятся в `gui_client/`, поэтому действует client loader + canonical WIP doc.

## 2026-07-10 — Post-migration hardening

| Изменение | Результат |
| --- | --- |
| `инструкция.txt` переименован в `MCOS.md` | MCOS стал canonical Markdown document; все AGENTS/index/policy links обновлены |
| MCOS mechanically formatted | title, source priority, section headings, TOC и stable appendix anchors; философский/нормативный текст не сокращался на этом этапе |
| Scientific `Qt-only` wording пересмотрен | документация теперь говорит: current implementation is Qt-based; будущий Web/SDL editor архитектурно не запрещён |
| AGENTS size review | root 94 строки; local loaders 18–38 строк; повторяющихся содержательных строк между loaders не найдено |
| Phase 4 structure audit | выполнен proposal-only; ADR/glossary/merge/move изменения не применялись автоматически |

## Phase 1 baseline, сохранённый migration

Первичный аудит создал `docs/codex` и восемь AGENTS, классифицировал existing documentation и добавил current warnings в mixed/legacy first-party docs. Phase 2 не откатывала эти изменения; она устранила дубли, обновила архитектуру по Scientific WIP и превратила набор документов в связанную canonical system.

## Правило будущих записей

Добавлять запись, если создан/удалён документ, изменилась его роль/status/canonical ownership, исправлено архитектурное противоречие или переработана система AGENTS. Обычные wording/link fixes отдельной строкой не накапливать.

## 2026-07-11 — Scientific Object Editor crash follow-up

| Документ | Изменение |
| --- | --- |
| `verification-report.md` | Добавлен crash follow-up: stale measurements при замене молекулы, fix и результаты smoke |
| `scientific-object-editor.md` | Зафиксировано правило очистки/валидации atom-index-dependent measurements при structure replacement |
| `build-and-test.md` | Добавлен regression smoke для replacement path после distance/angle/torsion |
