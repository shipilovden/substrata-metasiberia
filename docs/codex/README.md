# Codex guide for Metasiberia

Это вход в актуальные рабочие правила для Codex. Он не заменяет код: при конфликте код и подтверждённые build/runtime evidence имеют приоритет.

## Быстрый маршрут

1. Прочитать корневой `AGENTS.md` и ближайший локальный `AGENTS.md`.
2. Для неизвестной области открыть [project-index.md](project-index.md), затем [project-map.md](project-map.md) и [search-guide.md](search-guide.md).
3. Для изменения выбрать минимальную релевантную проверку в [build-and-test.md](build-and-test.md).
4. Для сложной задачи использовать [ORCHESTRATION.md](ORCHESTRATION.md); для делегации — [DELEGATION_CONTRACT.md](DELEGATION_CONTRACT.md).

## Рабочие документы

| Вопрос | Документ |
| --- | --- |
| Архитектура, owners и contracts | [architecture.md](architecture.md), [component-relations.md](component-relations.md) |
| Состояние и известные WIP | [current-state.md](current-state.md), [engineering-debt.md](engineering-debt.md) |
| Данные, runtime и secrets boundary | [data-map.md](data-map.md) |
| Проверки и команды риска | [build-and-test.md](build-and-test.md), [TESTING_AND_ACCEPTANCE.md](TESTING_AND_ACCEPTANCE.md) |
| Code review | [REVIEW_PROTOCOL.md](REVIEW_PROTOCOL.md) |
| Полный каталог и статус документов | [documentation-index.md](documentation-index.md) |

## Как не перегружать контекст

- Не читать MCOS, historical reports или feature research по умолчанию. Они нужны только при конкретном вопросе, который не покрывают краткие current документы.
- Использовать точный `rg`, определения символов и прямых consumers до расширения области чтения.
- Применять роли и параллелизм только для независимых областей и только если это уменьшает работу или риск.

## Статусы знания

Документы различают implemented, partial, WIP, plan, experimental, historical и unknown. Не превращать WIP или plan в утверждение о готовой функции.
