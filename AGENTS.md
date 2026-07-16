# AGENTS.md — Metasiberia

## Назначение

Metasiberia — форк Substrata: единая система native/web client, realtime server, embedded Server Website/Admin, shared contracts, assets и pipelines.

Этот файл — компактный loader. Факты проекта находятся в [`docs/codex/`](docs/codex/), инженерный стандарт — в [`docs/codex/MCOS.md`](docs/codex/MCOS.md). Ближайший локальный `AGENTS.md` дополняет этот файл и не отменяет source-of-truth hierarchy.

## Источники истины

1. Рабочий код текущей `master` и build manifests.
2. Реальная architecture и data flow.
3. Подтверждённые project/runtime данные.
4. MCOS как инженерный стандарт.
5. Актуальные документы `docs/codex`.
6. Plans/legacy/history — только как намерение или свидетельство.

AGENTS не являются источником истины. Цепочка сопровождения `код -> docs/codex -> MCOS -> AGENTS` показывает, как знания доходят до loader, а не меняет приоритет разрешения конфликтов.

Код имеет абсолютный приоритет для реализации. Production status всегда проверяется заново. WIP рабочего дерева не считать committed baseline.

## Обязательное начало задачи по коду

```powershell
git -C C:\programming\substrata status --short --branch
git -C C:\programming\substrata branch -vv
```

- Не удалять, не откатывать, не форматировать и не перезаписывать чужие dirty/untracked файлы.
- Не делать pull/switch/merge/rebase/reset/force-push, пока не понятны локальные изменения и divergence.
- Перед изменением открыть ближайший local `AGENTS.md`.

## Порядок исследования

1. Определить subsystem и тип задачи.
2. Открыть [`project-index.md`](docs/codex/project-index.md).
3. Выбрать owner в [`project-map.md`](docs/codex/project-map.md).
4. Использовать [`search-guide.md`](docs/codex/search-guide.md).
5. Построить producer/consumer/data boundary.
6. Только затем читать минимальный source range.

Не выполнять повторный полный аудит без architecture change, системного расхождения или прямого задания.

## Главные архитектурные правила

- Native и Web Client используют одну `gui_client` codebase; `webclient/` — browser shell.
- Realtime, HTTP/HTTPS/WebSocket, Server Website и `/admin*` — логические подсистемы одного `server` process.
- `shared/` — общий wire/model/disk contract; изменение обычно затрагивает client и server.
- Public Website `metasiberia.com` находится вне подтверждённого repo source boundary.
- Persistence — versioned custom binary state, не SQL.
- Source assets, generated output и deployed copies — разные stages.
- Scientific Object Editor — официальный WIP; текущая UI-реализация основана на Qt: generic `WorldObject`, marker/JSON в `content`, server хранит его непрозрачно.

Подробности: [`architecture.md`](docs/codex/architecture.md), [`component-relations.md`](docs/codex/component-relations.md), [`scientific-object-editor.md`](docs/codex/scientific-object-editor.md).

## Правила изменений

- Минимальный patch без попутного refactor/rename/format.
- Сохранять локальный C++17 style, RAII, ownership и lock contracts.
- New source/UI files регистрировать в CMake/MOC/UIC.
- Protocol/serialization/state changes требуют compatibility plan и всех consumers.
- Client editor не заменяет server permissions/validation.
- Не патчить external `glare-core`, Winter или vendor tree как скрытый workaround.
- Не выводить/коммитить credentials, tokens, cookies, keys, private user data.
- Deploy/restart/DNS/firewall/state restore/migration/publish/external messages — только по явному подтверждению.

## Token Intelligence

- Docs -> project map -> exact `rg` -> definition/direct consumers -> новая область только при evidence.
- Большие files читать диапазонами; generated/binary/minified/vendor/build caches исключать до необходимости.
- Переиспользовать canonical knowledge вместо копирования.
- Самая узкая достаточная проверка выполняется первой; full build/test только по радиусу изменения.
- Субагенты — только когда разрешены и области действительно независимы.

Политика: [`token-policy.md`](docs/codex/token-policy.md).

## Документация

- Разделять implemented, partial, WIP, plan, experimental, obsolete, historical и unknown.
- Один факт имеет один canonical document; остальные ссылаются.
- При architecture/contract/data/workflow change обновлять профильный `docs/codex` документ и [`current-state.md`](docs/codex/current-state.md).
- При смене роли документа обновлять [`documentation-index.md`](docs/codex/documentation-index.md) и [`documentation-changelog.md`](docs/codex/documentation-changelog.md).
- Не переписывать release/incident history под настоящее.

## Проверка и завершение

- Команды и side effects: [`build-and-test.md`](docs/codex/build-and-test.md).
- Windows Qt-клиент считается собранным для владельца только когда обновлён канонический `C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe`; `C:\programming\substrata_output` является legacy output и не подходит для launch/UI verification.
- Если изменён `server/**`, `shared/**` с серверными читателями/писателями или общий протокол/сериализация, обязательно в той же задаче пересобрать локальную цель `server` (`cmake --build C:\\programming\\substrata_build_qt --config RelWithDebInfo --target server -j 8`) и проверить бинарь. Клиентская сборка без server build не считается завершением такой задачи. Сервер не запускать и не деплоить без прямой просьбы пользователя.
- Важно: production server — Ubuntu/Linux `metasiberia-server`, а не Windows. Для серверных правок обязательны Linux-сборка и выкладка по [`SERVERS_AND_EXCHANGE.md`](docs/SERVERS_AND_EXCHANGE.md): `/srv/metasiberia/build/master` → `/srv/metasiberia/output/test_builds/server` → release → `/srv/metasiberia/releases/current` → `metasiberia-server.service`. Windows `server.exe` допустим только как локальный compile-check и никогда не обновляет production.
- SSH/sudo note: ошибка `sudo: Authentication failed` при передаче пароля через Windows OpenSSH pipe может быть вызвана TTY/кавычками, а не неверным паролем. Сначала проверить тот же доступ через PuTTY/plink с подтверждённым host key; не менять секрет и не записывать его в docs. Рабочий plink-доступ уже подтверждён для `metasiberia-server`.
- Перед Windows Qt build проверить Ruby и отсутствие другой активной сборки в том же build tree; timeout инструмента не считать завершением процесса. Idle MSBuild node-reuse workers с `/nodemode:` и завершившимся coordinator не считать отдельной активной сборкой и не убивать. Точный recovery для `LNK1136` описан в `build-and-test.md`.
- Не запускать `gui_client.exe`, включая runtime smoke, без прямой просьбы пользователя; compile/link/runtime-copy и ручной UI-запуск — разные разрешения.
- Docs-only: links, paths, terminology, diff/readback; C++ build не нужен.
- Итог: результат, изменённые области, проверки, непроверенное/риски и manual follow-up.
- Не публиковать command transcript и длинные успешные логи.

## Основная навигация

[`system-overview.md`](docs/codex/system-overview.md) · [`project-index.md`](docs/codex/project-index.md) · [`current-state.md`](docs/codex/current-state.md) · [`data-map.md`](docs/codex/data-map.md) · [`decisions.md`](docs/codex/decisions.md) · [`documentation-index.md`](docs/codex/documentation-index.md)
