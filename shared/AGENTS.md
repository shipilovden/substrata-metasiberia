# Локальные инструкции: `shared`

Дополняет [корневой AGENTS.md](../AGENTS.md). Канон contracts: [component-relations.md](../docs/codex/component-relations.md#contracts-с-максимальным-радиусом).

## Граница

- `Protocol.h`/`MessageUtils.h` — wire IDs, version и framing.
- `WorldObject`, `WorldMaterial`, `WorldSettings`, `Parcel`, `Avatar`, `Resource`, `GearItem` и IDs участвуют в network и/или disk contracts.
- Sources компилируются в consumers; это не independent library boundary.

## Уникальные правила

- Не переиспользовать message ID и не менять payload без all readers/writers.
- Новые fields требуют version/capability/trailing-field strategy и bounded read.
- Сохранять length limits, enum/ID stability и deterministic serialization.
- Перед изменением найти client/server/web/bot и persistence consumers.
- `WorldObject::content` max 10 000 bytes ограничивает Particle/Scientific marker payloads.
- Scientific special type/schema нельзя добавлять скрыто: нужен ADR, compatibility и old generic-object behavior.
- Version headers обновляются согласованно по release policy.

Shared change обычно требует client + server checks и old fixture при disk-format impact; команды — в [build-and-test.md](../docs/codex/build-and-test.md).
