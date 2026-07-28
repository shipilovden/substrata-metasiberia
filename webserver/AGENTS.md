# Локальные инструкции: `webserver`

Дополняет [корневой AGENTS.md](../AGENTS.md). Это source module executable `server`, не standalone service.

## Owners

- Router: `WebServerRequestHandler.cpp`.
- Public/account/auth: `MainPageHandlers`, `LoginHandlers`, `AccountHandlers`.
- Admin: `AdminHandlers`, routes `/admin*`.
- Disk data: `WebDataStore` + `../webserver_public_files`, `../webserver_fragments`, Web Client output.

## Уникальные правила

- Начинать с exact GET/POST path и handler; учитывать route order, redirects и WebSocket upgrade.
- State-changing route сохраняет validation, permission/auth guard, escaping, lock и response/redirect contract.
- Logged-in user не равен admin; каждый privileged handler проверяет current admin rule.
- `/` — Server Website, `/admin*` — Admin Panel; `metasiberia.com` — внешний Public Website.
- Не выдумывать separate REST/OpenAPI backend; current handlers напрямую используют server state.
- Не дублировать content в C++, fragment и JS без определения source of truth.
- Source, Web Client output и deployed disk copies проверяются как разные stages.

Проверка web C++ выполняется через target `server` и local route/auth flow по [build-and-test.md](../docs/codex/build-and-test.md). Production deploy не является тестом.
