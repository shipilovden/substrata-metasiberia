# Testing and acceptance

Этот документ определяет gates, а [build-and-test.md](build-and-test.md) остаётся каноническим источником подтверждённых команд и их side effects.

## Выбор проверки

1. Выполнить syntax/static sanity для изменённого типа файла.
2. Собрать минимальный affected target, когда менялся компилируемый код.
3. Запустить существующий целевой test/smoke только если он относится к изменению и его side effects приемлемы.
4. Добавить manual flow для UI/runtime изменения.
5. Расширять до client/server/compatibility проверки при изменении shared contract или high-risk boundary.

Не заменять проверку только `git diff`; не запускать весь набор тестов без связи с задачей.

## Минимальные gates

| Изменение | Минимум |
| --- | --- |
| Docs/instructions | Markdown links, paths, readback, `git diff --check` |
| Local C++/Qt UI | affected target build; manual flow описан или выполнен по разрешению |
| Server/webserver | target `server`; route/state boundary review по риску |
| Shared/protocol/serialization | client + server consumers, compatibility evidence и review |
| Build/config/dependency | config syntax, affected configure/build и review |
| Runtime/production | только по явному разрешению; deployment не считается тестом |

## Acceptance

Работа готова, когда достигнуты критерии пользователя, нет известных blocker/high defects, завершены соразмерные проверки, сохранены пользовательские изменения и проверен собственный diff. Финальный отчёт включает:

- результат и изменённые области;
- выполненные проверки и результат;
- непроверенные границы, риски и manual follow-up;
- отсутствие commit/push/deploy, если они не были запрошены.
