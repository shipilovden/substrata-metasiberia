# Development Intelligence

Назначение: подтверждённые инженерные правила и стандартный workflow для изменений Metasiberia.

Проверено: 2026-07-10 по code/build graph и MCOS. Локальный formatter/linter standard в repo не найден; сохраняется стиль затронутого кода.

## Стандартный workflow

1. Выполнить обязательные `git status --short --branch` и `branch -vv`.
2. Зафиксировать dirty/untracked файлы пользователя; не удалять и не откатывать их.
3. Определить subsystem, data owner, producers/consumers и риск.
4. Открыть ближайший `AGENTS.md` и минимальные docs через [project-index.md](project-index.md).
5. Найти exact symbol/route/message/marker по [search-guide.md](search-guide.md).
6. Подтвердить причину или точку расширения до изменения.
7. Изменить минимальный набор файлов без попутного refactor/format.
8. Выполнить самую узкую достаточную проверку из [build-and-test.md](build-and-test.md).
9. Расширить проверку только по compatibility/runtime риску.
10. Обновить постоянную документацию, если изменилось устойчивое знание.
11. Завершить результатом, проверками, непроверенным и ручными действиями.

## Архитектурная ответственность

- Перед изменением назвать проблему, затронутые подсистемы, новые зависимости и ожидаемое влияние через несколько лет.
- Предпочитать минимальное изменение, которое решает задачу без переписывания соседних подсистем.
- Если есть несколько вариантов, сравнить complexity, maintainability, extensibility, compatibility и implementation cost.
- Не возражать владельцу проекта из-за вкуса; возражать только при объективном риске архитектуры, данных, производительности, безопасности или расширяемости.
- Не превращать эксперимент в основную архитектуру без подтверждённой необходимости и плана сопровождения.

## Работа с долгом

- Найденный долг не исправлять автоматически, если он не входит в текущую задачу.
- Классифицировать проблему как Engineering, Documentation, Build, UX, Performance или Security Debt.
- Для подтверждённого долга указать описание, причину, последствия, приоритет и предлагаемое решение.
- Постоянный register: [engineering-debt.md](engineering-debt.md).
- Идею улучшения без подтверждённой проблемы фиксировать как рекомендацию в [verification-report.md](verification-report.md), а не как debt.

## Ownership и границы

- Deployment/data boundary важнее directory boundary.
- `webserver`/Admin — часть process `server`; Web Client — build mode общей client codebase.
- `shared` change считается межкомпонентным, пока не доказано обратное.
- External Public Website/TheRift не менять через repo owners без source evidence.
- Не патчить local `glare-core`, Winter или vendored dependency как скрытый обходной путь.

## C++

- C++17 — текущий standard.
- Сохранять локальные tabs/braces/naming/include order.
- RAII и понятный owner для memory, threads, sockets, GPU/audio/Qt resources.
- Использовать существующие `Reference<>`, lock abstractions и Qt parent ownership последовательно.
- Не добавлять copies/allocations/logging в frame, packet, audio и shared-state hot paths без оценки.
- Новый common layer создавать только при реальных нескольких consumers.
- Новые `.cpp/.h/.ui` регистрировать в target CMake и MOC/UIC rules.

## Validation и ошибки

- Проверять lengths, IDs, enums, URLs/paths, ownership, permissions и placement до mutation.
- На boundary сохранять существующий protocol/HTTP/error style.
- Не глотать исключение без безопасного fallback или diagnostic context.
- Не включать credentials, tokens, session IDs или user content в logs/errors.
- Corrupt persistence — recovery task с backup/copy, не обычный rebuild.

## Protocol и HTTP contracts

- Message ID уникален и стабилен; readers/writers и all peers меняются согласованно.
- Новые поля требуют protocol/model version, capability или безопасного trailing layout.
- Сохранять bounds и length prefixes; документировать old/unknown peer behavior.
- Private chat маршрутизируется по avatar UID первично, name/login — fallback.
- Route path/form/response — contract с forms/JS; state-changing admin route требует auth/admin guard.
- Отдельного REST boundary нет: handler mutation анализируется вместе с server locks/state.

## Persistence и concurrency

- `server_state.bin` — high-risk versioned binary record DB.
- Schema/record/model change требует old read path либо migration/version strategy.
- Проверка migration выполняется только на копии fixture/snapshot.
- Соблюдать `WorldStateLock` annotations и lock ordering.
- Не выполнять blocking disk/network/external API под global state lock без доказанной необходимости.
- Не расширять lock scope попутно; оценивать race, lifetime, deadlock и global scans.

## Client/UI/editors

- Qt widget lifetime/action/state связывать с owner; `.ui/.h/.cpp`, translation и CMake обновлять согласованно.
- Common gameplay/render/network logic не должна случайно получить Qt dependency.
- Editor предлагает изменение, но server повторно проверяет permissions/read-only/placement.
- Не объявлять feature готовой по наличию UI, если data/server/runtime path отсутствует.
- Сохранять Object Editor selection, undo/dirty flags, transform gizmos и authoritative update semantics.

## Scientific Object Editor (WIP)

Каноническая модель и статус: [scientific-object-editor.md](scientific-object-editor.md).

- Scientific Object остаётся generic `WorldObject`, пока отдельный shared/server type не принят ADR.
- Marker `metasiberia_scientific_object_v1` и JSON — текущий provisional envelope; изменение marker/fields требует compatibility plan.
- Общий `WorldObject::MAX_CONTENT_SIZE` 10 000 байт является жёсткой network/persistence границей. UI/serializer не должны создавать принимаемое локально, но неотправляемое состояние.
- Не добавлять название file format/database/provider как заявление о рабочем adapter.
- Mock/template result должен быть явно помечен и не смешиваться с network/AI result.
- Generated code нельзя исполнять без отдельного sandbox, resource limits, dependency и security design.
- Scientific data и визуальное представление должны быть разделены; сохранять existing temp OBJ -> `.bmesh` -> checksum resource conversion и проверять upload/reload.
- Local absolute file/temp path нельзя оставлять в shared object после conversion.
- API key не должен попадать в `WorldObject::content`, protocol, logs или docs. Текущий QSettings path требует отдельного credential-storage review до production-ready AI integration.
- Новая discipline/type проходит model -> bounded data -> visualization -> editor -> serialization -> compatibility -> docs; не создавать отдельный scene/server/persistence параллельно.
- Текущая реализация Scientific Editor основана на Qt; это не запрещает будущий Web/SDL UI, но parity нельзя объявлять без отдельной реализации и проверки.

## Rendering, XR, map и resources

- XR optional/native-only; desktop path сохраняется при `XR_SUPPORT=OFF` и bootstrap failure.
- Не сдвигать camera для скрытия local avatar; скрывать model/head только в active first-person XR.
- Не добавлять дополнительный full desktop render поверх eye renders без performance evidence.
- Physical Mercator map layer не зависит от avatar altitude или MiniMap zoom.
- Resource hash identity immutable: bytes не меняются под прежним checksum URL.
- Basis/original fallbacks проверяются по затронутым desktop/web/XR consumers.

## Assets и dependencies

- Editable master хранить в `source_resources`/доказанном source; derivative — в runtime stage.
- Не редактировать binary/minified/generated artifact как source.
- Проверять license/provenance fonts/media/models.
- Изменение asset path включает copy/preload/web/runtime consumers.
- Dependency version update — отдельная задача с build/runtime/license review.

## Проверка

- Narrow static/target check first; full build только по реальному радиусу.
- Shared/protocol/persistence: client + server + compatibility/old fixture.
- UI: compile + manual flow; Web Client: Emscripten/local delivery; XR: On/Off + desktop/HMD по scope.
- CI configure-only не доказывает compile/test/runtime.
- В рамках текущей documentation migration build/test запрещены; допустимы link/path/term/diff checks.

## Release и production

- Version headers изменяются согласованно с release notes/tag policy.
- Installer upload остаётся ручным по canonical release policy.
- Deploy, restart, DNS/firewall, state migration/restore, external publish/messages требуют явного подтверждения.
- Production Linux использует ELF `server`, не Windows `server.exe`.

## Документация

- Разделять implemented, partial, WIP, planned, experimental, obsolete, historical и unknown.
- Один факт имеет один canonical document; другие документы ссылаются.
- Runtime status датировать и перепроверять live.
- Architecture/contract/data/workflow change обновляется в той же задаче.
- Historical incident/release content сохранять, опасный stale advice помечать заменой.

Обновлять rules при новом универсальном engineering constraint; subsystem-only правило хранить в локальном `AGENTS.md` или профильном документе.
