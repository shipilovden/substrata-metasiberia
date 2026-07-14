# Search Guide: Token Intelligence

Назначение: найти минимальную область кода и доказательств для задачи без повторного аудита репозитория.

Проверено: 2026-07-14. Применять после корневого/локального `AGENTS.md` и [project-index.md](project-index.md).

## Четыре уровня исследования

| Уровень | Действие | Когда остановиться | Результат |
| --- | --- | --- | --- |
| 1. Документация | открыть один канонический документ по вопросу; при неясном термине — [glossary.md](glossary.md) | owner, contract и статус уже ясны | список терминов/путей для проверки |
| 2. Карта | использовать [project-map](project-map.md), [relations](component-relations.md), [data-map](data-map.md) | определены producers/consumers и physical boundary | минимальная область source |
| 3. Минимальный код | `rg` по точному symbol/route/message/marker; открыть definition и прямых consumers | причина/contract подтверждены | evidence с path/symbol |
| 4. Новая область | расширить к соседнему component или external boundary | только при новой зависимости/неопределённости | обновлённая карта знаний |

Не переходить на уровень 4 из любопытства. Полный аудит нужен только после крупной реорганизации, contract migration, системного расхождения docs/code или прямого задания владельца.

## Базовый алгоритм

1. Сформулировать вопрос одним предложением.
2. Классифицировать его: UI, protocol, state, route, resource, pipeline, runtime или docs.
3. Найти owner в [project-map.md](project-map.md).
4. Выписать возможные producers/consumers из [component-relations.md](component-relations.md).
5. Выполнить один точный `rg`.
6. Открыть definition и непосредственный call/handler/serializer.
7. Зафиксировать подтверждённое, неизвестное и требуемую проверку.
8. Расширить область только если найденная зависимость этого требует.

## Маршрутизация задач

| Задача | Сначала открыть | Минимальный source поиск | Добавить только если найдено влияние |
| --- | --- | --- | --- |
| Термин или boundary | [glossary.md](glossary.md) | не нужен, если профильный документ уже даёт ответ | source evidence только при споре с current code |
| Qt UI/menu/dialog | `gui_client/AGENTS.md`, конкретный widget/action | имя class/slot/objectName в `.h/.cpp/.ui` | `MainWindow`, translations, CMake/MOC |
| SDL/Web UI | `webclient/AGENTS.md`, `SDLUIInterface`/shell | `EMSCRIPTEN`, `USE_SDL`, конкретный event | common `GUIClient`, preload/WS route |
| Обычный Object Editor | `ObjectEditor.*`, selected object flow | signal/slot и изменяемое поле | shared serializer/server validation |
| Scientific Object WIP | [scientific-object-editor.md](scientific-object-editor.md) | `ScientificObject`, marker, `WorldObject::content` | `MainWindow`, CMake, generic object update/server limits |
| Scientific providers/API | [scientific-data-providers.md](scientific-data-providers.md) | `searchPubChem`, `loadPubChemCID`, parser/cache symbol only after selected provider | official API docs, cache/storage only if implementation starts |
| Particles | `ParticleEmitterSettings.*`, `ParticleManager.*` | content marker/field, update loop | resources/audio, Object Editor, GUIClient |
| Procedural Tree Editor | [architecture.md](architecture.md#procedural-tree-editor-wip) | `TreeParams`, `TreeGenerator`, `TreeObject`, `TreeEditorPanel`, `on_actionAddTree_triggered` | generic model/resource path, runtime copy, server/reconnect only if affected |
| Protocol | `shared/AGENTS.md`, `Protocol.h` | message ID/name во всех first-party dirs | both peers, bots, compatibility/tests |
| Private chat | chat plan + `PrivateChatMessageID` | recipient UID send/read/route | UI history/tabs, production server evidence |
| Persistence | `server/AGENTS.md`, [data-map](data-map.md) | model reader/writer, `db_dirty`, migration | old fixture/backup/runbook |
| Server lifecycle | `Server.cpp` relevant symbol | listener/config/thread creation | embedded web/state/ops only if affected |
| HTTP/WS route | `webserver/AGENTS.md`, router | exact request path | handler, auth/lock, JS/form consumer |
| Admin | `/admin*` router + `AdminHandlers` | route and admin guard | model mutation, public assets |
| Server Website | handlers + public files/fragments | route, asset key, fragment name | WebDataStore/watcher/deploy source |
| Public Website | [system-overview](system-overview.md) | repo evidence for `metasiberia.com` | остановиться, если external source отсутствует |
| Resources/attachments | `Resource*`, upload/download owners | URL/hash/extension | handler, copy/preload, desktop/web/XR fallback |
| Map world | map docs + `MapWorld*` | tile namespace/route/world scale | web map, maintenance/service boundary |
| Rendering/XR | concrete renderer or `XR*` | call site/feature guard | shader/resource/glare-core API boundary |
| Build/configure | [build-and-test](build-and-test.md) | option/target in root + local CMake | wrapper/script/CI only as needed |
| CI | workflow + affected CMake | trigger/path/filter/step | external dependency refs |
| Release/installer | release pipeline | version headers/tag/helper | owner-authorised publish stage only |
| Production | operational runbook | dated current paths/services | live read-only check after explicit scope |
| Documentation | `docs/AGENTS.md`, docs portal | link/path/term | source evidence only for disputed facts |

## Scientific Object: минимальная карта поиска

| Вопрос | Точный вход |
| --- | --- |
| Как распознаётся объект? | `ScientificObjectSettings::contentMarker`, `isScientificObjectContent` |
| Какие поля сериализуются? | `ScientificObjectSettings.h`, `fromContent`, `serialiseToContent` |
| Как создаётся? | `MainWindow::on_actionAddScientificObject_triggered` |
| Как выбирается editor? | `MainWindow::setObjectEditorFromOb` |
| Как сохраняется? | `ScientificObjectEditor::toObject` -> обычный object update |
| Почему источник не должен подменять данные? | `setScientificSourceResult`, `load_status`, `data_origin`, `provenance_*` |
| Где built-in sample data? | `builtInMoleculeSamples`, только `Caffeine`/`Water`, не provider adapter |
| Где PubChem Phase 1/1.2 provider? | `searchPubChem`, `loadPubChemCID`, `loadPubChemCardSection`, `loadPubChemImage`, `pubchemGet`, `parsePubChemSdf` |
| Где molecule interaction layer? | `MoleculeViewportWidget::handleSceneRay`, `MainWindow::glWidgetMousePressed`, `runMoleculeInformationSmokeCheck`, `PeriodicTableModel` |
| Что обновляет molecule mesh при visual controls? | `safeFileStemForScientificObject`, `writeMoleculeOBJForSettings`, `MODEL_URL_CHANGED` |
| Где физика Scientific Object? | `applyScientificPhysics`, existing `WorldObject` collidable/dynamic/sensor/mass/friction/restitution |
| Где labels/animation runtime? | schema descriptors only; искать `ObjectType_Text`/`recreateTextGraphicsAndPhysicsObs` или tick/update adapter, не считать `show_labels` runtime support |
| Что знает server? | generic `CreateObject`/`ObjectFullUpdate`; special marker search должен дать no match |
| Где реальные adapters/AI calls? | сначала поиск implementation; UI combo/status entries не считать adapter |
| Какие API/форматы/limits у будущего provider? | [scientific-data-providers.md](scientific-data-providers.md), затем official docs выбранного источника |
| Какой размер payload? | `WorldObject::MAX_CONTENT_SIZE`; guard in `ScientificObjectEditor::toObject` |

## Точные шаблоны поиска

```powershell
rg -n "ExactSymbol" gui_client shared server webserver
rg -n 'request\.path == "/route' webserver\WebServerRequestHandler.cpp
rg -n "MessageName|MessageID" shared gui_client server screenshot_bot
rg --files gui_client | rg 'ScientificObject|Editor|Settings'
git ls-files '*.md'
```

Начинать с одного шаблона. Не объединять десятки общих терминов в первый запрос.

## Правила минимального чтения

- Большие `MainWindow.cpp`, `GUIClient.cpp`, `Server.cpp`, `WorkerThread.cpp` читать диапазоном вокруг найденного symbol.
- Header использовать для contract/fields, implementation — только для затронутой логики.
- Не читать все sibling editors для «сравнения»; выбрать один ближайший существующий pattern.
- Не открывать binary/font/model/image, generated CMake caches, minified vendor assets и lock-файлы как текст.
- Не исследовать vendored/external source, пока stack или API mismatch не указывает внутрь него.
- Не повторять уже подтверждённую информацию: ссылаться на канонический документ.
- Не переносить полные successful logs; сохранять command class, exit code и существенное сообщение.

## Stop rules

Остановить расширение исследования, когда:

- owner и data boundary установлены;
- найдено одно подтверждение реализации и все прямые consumers;
- дальнейшие каталоги не влияют на решение;
- вопрос упирается во внешний source/runtime, которого нет в scope;
- следующий шаг требует production mutation или новых полномочий.

## Сохранение новых знаний

Устойчивое знание записывать один раз:

- новый component/owner -> [project-map.md](project-map.md);
- cross-component термин -> [glossary.md](glossary.md);
- новый contract/flow -> [architecture.md](architecture.md) и [component-relations.md](component-relations.md);
- data/schema/storage -> [data-map.md](data-map.md);
- status/readiness -> [current-state.md](current-state.md);
- архитектурное решение -> [decisions.md](decisions.md);
- подтверждённый долг без текущего исправления -> [engineering-debt.md](engineering-debt.md);
- новый маршрут исследования -> этот Search Guide.

Не обновлять docs из-за временной гипотезы. Обновлять guide при повторяющемся поиске, новом owner или изменении entry point.
