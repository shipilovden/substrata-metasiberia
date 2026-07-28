# Локальные инструкции: `docs`

Дополняет [корневой AGENTS.md](../AGENTS.md). Портал: [codex/documentation-index.md](codex/documentation-index.md).

## Уникальные правила

- Отделять code fact, dated runtime snapshot, partial/WIP, plan, experiment, obsolete и history.
- MCOS задаёт стандарт, но не является описанием реализации; при конфликте исправлять MCOS/docs по code evidence.
- Один факт хранить в одном canonical document; остальные связывать ссылками.
- Historical release/incident docs не переписывать; опасный stale advice помечать current replacement.
- Wiki `_Sidebar.md`/`_Footer.md` сохраняют GitHub Wiki names/semantics.
- Не переносить secrets, tokens, cookies, private user content или sensitive captures.
- При смене role/status обновлять documentation portal и changelog.

## Проверка

Проверить relative links, repo paths, headings/anchors, terminology и provenance команд. Docs-only task не запускает build/test/runtime/deploy/publish.

Основные стандарты: [codex/MCOS.md](codex/MCOS.md), [codex/search-guide.md](codex/search-guide.md), [codex/token-policy.md](codex/token-policy.md).
