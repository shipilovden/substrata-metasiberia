# Локальные инструкции: `scripts`

Дополняет [корневой AGENTS.md](../AGENTS.md).

## Перед запуском

Прочитать parameters/defaults и классифицировать script: read-only, local-output, build, network, remote-write, destructive, publish или long-running.

## Уникальные правила

- Deploy/release/wiki publish, service/DNS/state/restore и external messages не запускать без явного задания.
- `v2`, `/root/cyberspace_server_state`, obsolete remote и automatic asset upload считать historical до проверки current runbook.
- Secrets передавать только через approved environment/local store; не добавлять в defaults/examples/logs.
- Сохранять dry-run/`-WhatIf`; destructive path должен быть validated и явно scoped.
- Emscripten preload script пересоздаёт local data/output и touch-ит external glare-core file: только isolated build dir.
- Не исправлять wrapper вне repo как скрытую часть repo patch.

Static syntax check допустим только если не выполняет side effects. Production endpoint не использовать как smoke без разрешения. Команды и риски: [build-and-test.md](../docs/codex/build-and-test.md).
