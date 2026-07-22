# Политика Qt 5 и Qt 6

Этот документ фиксирует, какую ветку и какой build tree использовать, когда владелец пишет «собираем Qt 5» или «собираем Qt 6».

## Ветки и изоляция

| Ветка | Qt | Wrapper | CMake build tree | Runtime output |
| --- | --- | --- | --- | --- |
| `master` | Qt 5.15.16 | `C:\programming\qt_build.ps1` | `C:\programming\substrata_build_qt` | `C:\programming\substrata_output_qt` |
| `qt6-integration` | Qt 6 MSVC 2022 | `C:\programming\qt6_build.ps1` | `C:\programming\substrata_build_qt6` | `C:\programming\substrata_output_qt6` |

`master` остаётся стабильной canonical Qt 5 веткой. Qt 6 экспериментальная/интеграционная работа выполняется только в `qt6-integration`; Qt 6 не должен менять default Qt 5 wrapper или Qt 5 output.

Каждый workflow обязан конфигурировать собственный build tree. Нельзя переключать Qt-версию через переменную окружения и затем использовать старый tree с `-SkipConfigure`: CMake cache, generated Visual Studio project и runtime copy должны соответствовать одной Qt-версии.

CEF/WebView включён в canonical native workflow обеих веток, если локальная CEF distribution и соответствующий wrapper доступны. UI/WebView и `gui_client.exe` не запускаются автоматически: после compile/link/runtime-copy проверку 3D-браузера выполняет владелец.

## Команды владельца

«Собираем Qt 5» означает:

```powershell
git -C C:\programming\substrata switch master
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1
```

«Собираем Qt 6» означает:

```powershell
git -C C:\programming\substrata switch qt6-integration
powershell -ExecutionPolicy Bypass -File C:\programming\qt6_build.ps1
```

Qt 6 wrapper обязан явно проверить Qt 6 MSVC 2022 и завершиться с ошибкой, если Qt 6 не установлен; fallback на Qt 5 запрещён.

## Статус Qt 6

После синхронизации `qt6-integration` с `master` нужно отдельно подтвердить Qt 6 configure/compile/link. Наличие `#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)` в исходниках само по себе не доказывает совместимую Qt 6 сборку.
