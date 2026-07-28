# Локальные инструкции: `webclient`

Дополняет [корневой AGENTS.md](../AGENTS.md).

## Граница

- `webclient.html` — browser shell для Emscripten build target `gui_client`.
- Gameplay/UI/network C++ находится в `../gui_client`; generated JS/WASM/data не source.
- Embedded server отдаёт output через `/webclient`/`WebDataStore`; realtime идёт по WebSocket общим protocol.

## Уникальные правила

- Не редактировать generated JS/WASM/data.
- Resource URL учитывает preload FS, cache hashes и server path.
- Preload/hash scripts имеют side effects; запускать только по подтверждённому isolated workflow.
- Qt-only Scientific Object UI не считать Web Client feature без отдельной implementation/parity decision.
- Historical v2 deploy paths/scripts не использовать как current production flow.

Emscripten compile + local HTTP/WSS smoke требуются только когда Web scope реально затронут; иначе явно отметить непроверенное. См. [build-and-test.md](../docs/codex/build-and-test.md).
