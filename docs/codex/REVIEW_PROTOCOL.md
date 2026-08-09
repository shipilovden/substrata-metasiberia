# Review protocol

## Когда нужен независимый review

Review обязателен для изменений с высоким риском: protocol/serialization/persistence, network/authentication, concurrency/locking, memory ownership, filesystem/destructive logic, build/dependency configuration, public API, rendering core или security-sensitive paths.

Для локального typo, docs-only исправления или механической правки без поведения review не является обязательным.

## Задача Reviewer

Reviewer получает пользовательскую цель, scope и готовый diff. Он не переписывает решение и не требует косметический refactor без риска. Он проверяет:

- соответствие заданию и scope;
- correctness, null/error handling и resource lifetime;
- C++/Qt ownership, threading/locks, protocol/persistence compatibility;
- security, permissions и external side effects;
- регрессии, accidental API breakage и недостающие проверки.

## Формат замечания

```text
SEVERITY: blocker | high | medium | low
FILE: path
LOCATION: symbol or line
PROBLEM: concrete defect
WHY IT MATTERS: impact and triggering condition
RECOMMENDED FIX: smallest safe correction
```

Замечание без конкретной причины и воспроизводимого риска не является gate.

## Разрешение результатов

Orchestrator исправляет blocker/high, оценивает medium по контексту и может оставить low как follow-up. После существенной коррекции повторяет затронутую verification; повторный review нужен, если исправление меняет high-risk logic.
