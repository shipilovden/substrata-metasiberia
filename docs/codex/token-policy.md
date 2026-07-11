# Политика Token Intelligence

Назначение: уменьшать стоимость каждой следующей задачи за счёт минимального чтения, повторного использования подтверждённых знаний и соразмерной проверки.

Основание: MCOS Appendices G/H. Проверено: 2026-07-10.

## Главный принцип

Токены тратятся на неизвестное и решение задачи, а не на повторное описание уже картографированного проекта. Экономия не оправдывает потерю producer/consumer, compatibility или safety boundary.

## Уровни исследования

### Уровень 1 — готовое знание

Открыть `AGENTS.md`, [project-index.md](project-index.md), при необходимости [glossary.md](glossary.md), и один канонический документ. Использовать для локальных задач, статуса и известных workflows.

Выход: owner, contract, статус, минимальная проверка. Код можно не читать, если запрос только о подтверждённой документации.

### Уровень 2 — карта подсистемы

Добавить [project-map.md](project-map.md), [component-relations.md](component-relations.md) или [data-map.md](data-map.md). Использовать, когда нужно определить физические consumers или data flow.

Выход: ограниченный список каталогов/symbols.

### Уровень 3 — минимальный source

Точный `rg`, definition, непосредственный caller/handler/serializer. Это обычный предел bugfix/feature/documentation verification.

Выход: evidence и область patch. Не читать component целиком, если вопрос решён.

### Уровень 4 — новое исследование

Расширение к неизвестной подсистеме, external boundary или system-wide audit. Допустимо при новой зависимости, архитектурной migration, повторяющемся расхождении docs/code или прямом указании владельца.

Выход обязан обновить постоянную карту знаний, чтобы уровень 4 не повторялся по той же причине.

## Радиус задачи

| Радиус | Пример | Максимальная нормальная область |
| --- | --- | --- |
| Локальный | текст, один widget, одна doc link | один owner + narrow check |
| Компонентный | editor feature, route, server handler | component + прямые contracts |
| Межкомпонентный | protocol, resource, persistence | все producers/consumers + compatibility |
| Архитектурный | новый deployable/schema/pipeline | plan, decisions, docs, staged verification |
| Аудит | неизвестная система/крупное расхождение | полный scope, только по отдельной причине |

Reasoning, число файлов, глубина чтения и проверка должны соответствовать радиусу, а не размеру prompt.

## Правила минимального чтения

- Сначала exact symbol/route/message/marker, затем range.
- Не читать один и тот же документ повторно в одной логической задаче.
- Header — для interface; implementation — для конкретного поведения.
- Один ближайший pattern лучше обзора всех похожих classes.
- Generated, binary, minified, lock, build cache и vendor code исключать до evidence.
- Large logs сокращать до первой причины и релевантного контекста.
- Git history читать только если вопрос требует происхождения решения.
- Production runbook не читать для локальной UI-задачи.

## Повторное использование знаний

- Ссылаться на канонический документ вместо копирования раздела.
- Терминологию брать из [glossary.md](glossary.md), если слово повторяется между подсистемами.
- Новое устойчивое знание добавлять в документ, владеющий этим вопросом.
- В `AGENTS.md` хранить маршрут и обязательное правило, но не объяснение архитектуры.
- В changelog фиксировать изменение роли/достоверности, а не журнал каждой опечатки.
- Historical incident/release docs не превращать в текущий runbook.
- Временные hypotheses и task notes не переносить в MCOS.

## Стратегии экономии

1. **Route first:** определить вопрос и owner до source search.
2. **Evidence ladder:** docs -> map -> exact symbol -> consumers -> новая область.
3. **One-pass extraction:** за одно чтение выписать contract, owner, risks и unknowns.
4. **No duplicate scans:** использовать результаты текущего аудита, пока структура не изменилась.
5. **Narrow verification:** сначала links/static/affected target; расширять только по риску.
6. **Stable summaries:** сохранять пути/symbols и вывод, не transcript команды.
7. **Boundary stop:** external source/production без evidence обозначить как unknown, не домысливать.
8. **Documentation payoff:** дорогой аудит обязан снижать стоимость будущих задач.
9. **Debt capture:** найденный, но не исправляемый долг фиксировать в [engineering-debt.md](engineering-debt.md), а не расследовать повторно в каждой задаче.

## Scientific Object WIP

Не перечитывать весь `MainWindow`/server для scientific-задачи. Начинать с [scientific-object-editor.md](scientific-object-editor.md), marker/settings, `MainWindow` integration и общего `WorldObject` limit. Server-wide исследование нужно только при изменении existing generic object contract или при введении специального server/shared типа.

UI list database/provider/file formats не является evidence реализации. Это правило предотвращает дорогой поиск несуществующих adapters.

## Субагенты и параллельность

Использовать только если это разрешено пользователем/сессионными правилами и задача делится на независимые области. Нельзя поручать нескольким агентам повторный полный scan. Главный агент объединяет термины, contracts и противоречия.

## Команды и проверки

- Read-only discovery выполнять параллельно, если результаты независимы.
- Не запускать full build/test для docs-only задачи.
- Не повторять идентичную неудачную команду без новой гипотезы.
- Deploy/publish/migration/restore не являются проверкой и требуют отдельного разрешения.
- Итог содержит результат, выполненную проверку, непроверенное и риски — не полный command log.

## Когда нужен повторный аудит

- изменились top-level owners или deployment boundaries;
- появился новый protocol/persistence/data schema;
- добавлен отдельный API, database, website repository или runtime service;
- несколько задач обнаружили независимые расхождения документации с кодом;
- владелец прямо запросил аудит/migration.

Обычный feature, bugfix, локальный UI или docs patch не является причиной полного аудита.

Связанные документы: [search-guide.md](search-guide.md), [project-index.md](project-index.md), [glossary.md](glossary.md), [engineering-debt.md](engineering-debt.md), [MCOS.md](MCOS.md).
