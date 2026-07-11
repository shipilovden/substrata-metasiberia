# Локальные инструкции: `gui_client`

Дополняет [корневой AGENTS.md](../AGENTS.md). Архитектура client: [docs/codex/architecture.md](../docs/codex/architecture.md#native-client).

## Граница

- Target `gui_client`; Qt entry `MainWindow.cpp::main`, SDL/Emscripten entry `SDLClient.cpp::main`.
- `GUIClient` владеет общей world/network/resource/render логикой; Qt/SDL — UI boundaries.
- Emscripten использует common/SDL path. Не вносить Qt dependency в общий код без compile guard.

## Перед изменением

- Проверить `git diff -- gui_client` и сохранить active Particle/Scientific WIP.
- Искать конкретный widget/manager/symbol; большие `MainWindow.cpp`/`GUIClient.cpp` читать диапазонами.
- Для UI сопоставить `.ui/.h/.cpp`, action/owner, translations и CMake/MOC.

## Инварианты

- `USE_SDL=OFF` — Qt; `ON` — SDL/Web path.
- XR optional/native-only; desktop работает без runtime/SDK. Не лечить avatar visibility camera offset и не добавлять лишний full mirror render.
- Map world Mercator scale не зависит от avatar altitude/MiniMap zoom.
- Resource/Basis fallbacks учитывать desktop/web/XR и не возвращать white textures.
- Chat/private/attachments меняют protocol/server вместе с UI; recipient UID первичен.
- Новый editor сохраняет selection, undo/dirty flags, permissions, transform lifecycle и authoritative server validation.

## Scientific Object WIP

- Канон: [scientific-object-editor.md](../docs/codex/scientific-object-editor.md).
- Current envelope: generic `WorldObject` + marker `metasiberia_scientific_object_v1` + JSON in `content`.
- Общий limit `WorldObject::MAX_CONTENT_SIZE` — 10 000 bytes; контролировать итоговый payload.
- UI database/provider/file lists и mock/template output не являются working adapters.
- Scientific temp OBJ остаётся промежуточным: сохранять existing `GUIClient::objectEdited()` conversion в `.bmesh`, checksum URL и resource upload path; проверять reload отдельно.
- API key не сериализовать и не логировать; current QSettings storage требует review.
- Qt WIP не считать автоматически поддержанным SDL/Web/XR.

## Проверка

Выбирать минимальную строку матрицы в [build-and-test.md](../docs/codex/build-and-test.md). UI требует compile + manual flow; shared/protocol change добавляет server. В docs-only задаче build/runtime не запускать.
