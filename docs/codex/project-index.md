# Навигатор проекта Metasiberia

Назначение: основной стартовый индекс для инженерной задачи. Он направляет к минимальному документу и owner, не повторяя их содержание.

Проверено: 2026-07-10. Источник фактов — текущий code/build graph; runtime claims требуют отдельной проверки.

## Быстрый старт

1. Прочитать корневой и ближайший локальный `AGENTS.md`.
2. Выбрать область в таблице ниже.
3. Открыть один канонический документ и соответствующий owner в [project-map.md](project-map.md).
4. Использовать точный поиск из [search-guide.md](search-guide.md).
5. Расширять область только после найденной зависимости.

## Маршрутизация по вопросу

| Вопрос | Основной документ | Дополнение |
| --- | --- | --- |
| Что такое Metasiberia и какие продукты существуют? | [system-overview.md](system-overview.md) | [project-map.md](project-map.md) |
| Где находится компонент/entry point? | [project-map.md](project-map.md) | [search-guide.md](search-guide.md) |
| Как устроены process/build/data boundaries? | [architecture.md](architecture.md) | [component-relations.md](component-relations.md) |
| Кто producer/consumer конкретного contract? | [component-relations.md](component-relations.md) | [data-map.md](data-map.md) |
| Какие данные существуют и где они живут? | [data-map.md](data-map.md) | [architecture.md](architecture.md) |
| Какие scientific sources/API/providers использовать? | [scientific-data-providers.md](scientific-data-providers.md) | [scientific-object-editor.md](scientific-object-editor.md) |
| Как устроены инвентарь, экипировка, avatar preview и gear protocol? | [inventory-system.md](inventory-system.md) | [component-relations.md](component-relations.md) |
| Что означает термин или boundary? | [glossary.md](glossary.md) | профильный документ из глоссария |
| Что реально реализовано, WIP или запланировано? | [current-state.md](current-state.md) | [audit-report.md](audit-report.md) |
| Почему архитектура выглядит именно так? | [decisions.md](decisions.md) | [architecture.md](architecture.md) |
| Какие инженерные правила применить? | [development-rules.md](development-rules.md) | ближайший `AGENTS.md` |
| Как найти минимальный код? | [search-guide.md](search-guide.md) | [token-policy.md](token-policy.md) |
| Какие команды подтверждены? | [build-and-test.md](build-and-test.md) | `scripts/AGENTS.md` |
| Как сопровождать знания? | [MCOS.md](MCOS.md) | [documentation-index.md](documentation-index.md) |
| Где зафиксирован известный долг? | [engineering-debt.md](engineering-debt.md) | [verification-report.md](verification-report.md) |
| Что изменилось в документации? | [documentation-changelog.md](documentation-changelog.md) | [verification-report.md](verification-report.md) |

## Компонентные входы

| Область | Local loader | Source owner | Каноническое знание |
| --- | --- | --- | --- |
| Native/Web Client, UI, editors, XR | `gui_client/AGENTS.md` | `gui_client/` | [architecture: Native Client](architecture.md#native-client) |
| Scientific Object Editor WIP | `gui_client/AGENTS.md` | `ScientificObjectEditor.*`, `ScientificObjectSettings.*`, `MainWindow.*` | [scientific-object-editor.md](scientific-object-editor.md); provider/API reference: [scientific-data-providers.md](scientific-data-providers.md) |
| Gear Inventory / avatar equipment | `gui_client/AGENTS.md`, `server/AGENTS.md` | `GearInventoryPanel.*`, `AvatarGraphics.*`, `GUIClient.*`, `WorkerThread.cpp` | [inventory-system.md](inventory-system.md) |
| Realtime Server/state/persistence | `server/AGENTS.md` | `server/` | [architecture: server](architecture.md#realtime-server-и-persistence) |
| HTTP/WS/Server Website/Admin | `webserver/AGENTS.md` | `webserver/`, assets/fragments | [architecture: web](architecture.md#httpwebsocket-server-server-website-и-admin-panel) |
| Shared protocol/models | `shared/AGENTS.md` | `shared/` | [relations](component-relations.md#contracts-с-максимальным-радиусом) |
| Web Client shell | `webclient/AGENTS.md` | `webclient/` + client Emscripten branch | [project map](project-map.md#карта-основных-подсистем) |
| Build/assets/release/ops | `scripts/AGENTS.md` | CMake, `scripts/`, assets, `systemd/` | [build-and-test.md](build-and-test.md) |
| Documentation/Wiki | `docs/AGENTS.md` | `docs/` | [documentation-index.md](documentation-index.md) |

## Статусы ключевых подсистем

| Подсистема | Статус | Важная граница |
| --- | --- | --- |
| Native Qt client | основной | canonical Qt 5 path |
| SDL/Web client | реализован/ограниченный | общая codebase, отдельный toolchain |
| Realtime + embedded web server | основной | один process и общий state |
| Server Website/Admin | основной source | deployment отдельно от source/build |
| Public Website | внешний/неизвестный | source отсутствует |
| Scientific Object Editor | официальный WIP | generic object + marker/JSON; текущая UI-реализация основана на Qt |
| OpenXR | экспериментальный | optional, native-only, default OFF |
| Chat redesign | частично | protocol/server/UI должны меняться совместимо |
| Map world | реализован | physical Mercator scale отделён от MiniMap zoom |
| Disabled bots/installer | experimental/legacy | не считать root-build baseline |

## Основные неизменяемые различия

- Public Website `metasiberia.com` != Server Website `vr.metasiberia.com`.
- Server Website `/` != Admin Panel `/admin*`.
- Web Client != отдельная gameplay codebase.
- `webserver/` != standalone service.
- `shared/` != самостоятельная library/deployment boundary.
- Scientific Object UI type != новый shared/server object type.
- Plan/mock/placeholder != реализованный adapter или runtime.

## Полный портал документации

Все канонические, тематические, historical и legacy документы классифицированы в [documentation-index.md](documentation-index.md).

Обновлять навигатор только при изменении роли основного документа, owner component или статуса ключевой подсистемы.
