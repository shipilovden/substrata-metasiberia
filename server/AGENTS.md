# Локальные инструкции: `server`

Дополняет [корневой AGENTS.md](../AGENTS.md). Канон: [architecture.md](../docs/codex/architecture.md#realtime-server-и-persistence).

## Граница

- `Server.cpp::main()` запускает realtime TLS, UDP, world maintenance и embedded HTTP/HTTPS listeners.
- `ServerAllWorldsState` владеет authoritative world/user/resource/session/media/bot state и persistence.
- `WorkerThread*` обрабатывает client protocol; `../webserver/` — HTTP/WS routes того же process.

## Уникальные правила

- Соблюдать `WorldStateLock`, annotations и lock ordering; не выполнять blocking I/O под global lock без анализа.
- Не расширять hot network/world loops logging, allocations или global scans.
- Новый message: parsing, bounds, permissions, all peers и compatibility strategy.
- Disk model/record change: old reader или migration/version plan, copied fixture, backup/restore implications.
- Runtime `server_state.bin`, resources, media, config и credentials находятся вне repo; не использовать production state для проверки.
- Generic Scientific Object content server не интерпретирует; special validation/type требует отдельного ADR/shared change.
- Web/admin mutation анализировать как server state/concurrency change.

## Production

Production deploy/release symlink/restart/state operation — только с явным подтверждением и актуальным [runbook](../docs/SERVERS_AND_EXCHANGE.md). Linux production binary — ELF `server`, не `server.exe`.

Проверки выбирать в [build-and-test.md](../docs/codex/build-and-test.md); docs-only migration не запускает server/build/tests.
