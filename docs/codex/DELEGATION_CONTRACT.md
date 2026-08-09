# Delegation contract

Каждое поручение дочернему агенту должно быть коротким и самодостаточным. Передавать только контекст, необходимый для результата; не копировать весь диалог или репозиторий.

```text
ROLE:
Explorer | Implementer | Strong Implementer | Reviewer | Verification

GOAL:
Конкретный результат, который должен вернуться оркестратору.

FILES / SCOPE:
Разрешённые каталоги, файлы или symbols. Явно указать read-only, если это исследование.

CONTEXT:
Минимальные факты: subsystem, contract, имеющееся evidence, пользовательский критерий.

EXISTING PATTERN:
Файл, класс или механизм, которому нужно следовать, если он известен.

CONSTRAINTS:
Что не менять: protocol, public API, соседние owners, generated files, production, credentials и т. п.

DONE WHEN:
Наблюдаемые критерии готовности.

VERIFICATION:
Точные релевантные checks либо указание, что агент должен предложить их по evidence.

RETURN:
Изменённые файлы, findings/diff summary, выполненные проверки, риски и непроверенное.
```

## Дополнительные правила

- Explorer возвращает факты отдельно от предположений и указывает путь/символ для каждого существенного вывода.
- Implementer не меняет файлы за пределами scope. Если scope недостаточен, он сообщает блокер, а не расширяет задачу сам.
- Reviewer сообщает только реальные проблемы в формате из [REVIEW_PROTOCOL.md](REVIEW_PROTOCOL.md).
- Verification agent не запускает destructive, production или network-writing команды без отдельного разрешения.
- Orchestrator остаётся владельцем интеграции, конфликтов, final diff и ответа пользователю.
