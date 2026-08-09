# Миграция product identity: Substrata → Metasiberia

Статус: выполнена безопасная первая фаза.  Цель этой матрицы — не нулевое
число legacy-слов, а Metasiberia в пользовательской и собственной project
identity без разрыва работающих контрактов.

| Категория | OLD | NEW | Безопасно переименовать | Риск / зависимости | Действие |
|---|---|---|---|---|---|
| Product branding | `Substrata` в окне, веб-заголовках, инсталляторе | `Metasiberia` | Да, если строка не является URL, route или внешней интеграцией | Старые статические страницы могут быть исторической документацией | Обновлены runtime-журнал клиента/сервера, web-client title, installer URL и canonical landing page. |
| Project/application identity | CMake project `substrata` | `metasiberia` | Да | Reconfigure изменяет имя generated CMake project, но не targets | Переименован только `PROJECT_NAME`; `gui_client` и `server` сохранены. |
| Build targets и executables | internal build/output names `cyberspace_x64`, `gui_client.exe`, `server.exe` | прежние технические имена | Нет, не в этой фазе | wrappers, installer, CI, deployment и уже установленный runtime | Сохранены. Инсталлятор уже показывает Metasiberia и ведёт на `vr.metasiberia.com`. |
| C++ identifiers/classes/namespaces | `SubstrataLuaVM`, `SUBSTRATA_*`, `Cyberspace*` | прежние ABI/source identifiers | Нет | Большое дерево includes, downstream/private code, scripts | Сохранены как legacy implementation identifiers; публичные runtime strings заменяются точечно. |
| Network/protocol identifiers | `sub://`, `CyberspaceHello`, `CyberspaceProtocolVersion`, message IDs | прежние wire identifiers | Нет | Клиенты, серверы, боты, saved worlds и third-party implementations | Не менялись. Protocol version и serialisation contract остались byte-compatible. |
| Serialization/persisted identifiers | `server_state.bin`, resource layouts, object fields | прежние formats | Нет | Существующие worlds и user data | Не менялись. |
| User data/AppData/settings/cache paths | `Cyberspace`, QSettings `Glare Technologies/Cyberspace` | `Metasiberia` для чистой Qt установки | Только с fallback | Credentials, layout, cache, screenshots, Indigo/webcam data | `MetasiberiaPaths` выбирает существующий legacy data dir; новые установки получают `Metasiberia`. Qt settings импортируют отсутствующие ключи из legacy store один раз без перезаписи новых. SDL registry store намеренно сохранён legacy до отдельной миграции его SettingsStore API. |
| Server configuration | `substrata_server_config.xml`, `substrata_server_credentials.txt` | `metasiberia_server_config.xml`, `metasiberia_server_credentials.txt` | Да, только с fallback | Production state, secrets, operator automation | Сначала ищется новый файл, затем legacy. Ничего не переносится и не перезаписывается автоматически. Linux state `cyberspace_server_state` сохранён. |
| Server/deployment/service names | existing release, state, service, cache namespaces | прежние operational identifiers | Нет без согласованного deployment plan | systemd, backup, Caddy, screenshot bots, production state | Не менялись и не деплоились. |
| Documentation | current Metasiberia docs; historical Substrata docs and URLs | Metasiberia canonical copy, historical material labelled | Да только для owned current docs | Legal/history/upstream links | Landing page теперь ведёт на active fork. Старые legal/FAQ/parcel/server routes явно маркированы как historical, а не переписаны без проверки фактов. |
| Third-party/upstream/licensing | `glaretechnologies/substrata`, copyright, Qt licences | без замены | Нет | Attribution and licence obligations | Не менялись. |
| Repository/local filesystem paths | `C:\programming\substrata`, `substrata_build_*`, `substrata_output_*` | подготовить отдельную migration | Нет сейчас | Qt wrappers, CI, CMake cache, scripts, linked worktrees and external automation | Корень не переименован. См. раздел ниже. |

## Совместимость данных

`shared/MetasiberiaPaths.h` implements non-destructive selection, not a bulk
copy.  A `Metasiberia` directory wins for the client only when it contains
client data; a server-only `Metasiberia/server_data` directory cannot displace
an existing `Cyberspace` client directory.  Otherwise an existing
`Cyberspace` client directory or `Substrata/server_data` Windows server
directory is reused.  Only a clean installation creates the new path.

The Qt settings migration imports only keys that do not already exist in the
new store and writes `migration/legacy_cyberspace_settings_imported_v1` after a
successful sync.  This keeps stored credentials, UI layout and preferences
without printing or exporting their values.

## Intentionally retained legacy names

- `sub://`, protocol constants, packet/message IDs and serialisation formats;
- `SubstrataLuaVM`, `SUBSTRATA_*` compile/environment switches and historical
  source file names;
- `CYBERSPACE_OUTPUT`, output-directory components and server state layout;
- existing production service names, Linux `cyberspace_server_state`, map cache
  namespace and deployment scripts;
- BugSplat identifiers, pending confirmation that a Metasiberia application is
  configured in that external service;
- upstream links, copyright and licence text;
- historical Substrata documentation, old routes and archive asset filenames.

## Repository-directory migration gate

Renaming `C:\programming\substrata` is deliberately blocked until all of the
following are prepared in one controlled maintenance task: the external
`C:\programming\qt_build.ps1` wrapper, local CMake build/output trees,
environment variables, Git worktrees, CI defaults, Figma/site helpers and
operator scripts.  The current workspace also has linked worktrees and a dirty
untracked documentation set, so the rename is not safe in this change.
