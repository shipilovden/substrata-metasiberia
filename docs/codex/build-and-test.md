# Сборка, тестирование и команды риска

Назначение: канонический реестр подтверждённых форм команд, окружения, side effects и минимальной проверки по компонентам.

Проверено по CMake/scripts/CI: 2026-07-10. Windows/Qt wrapper `C:\programming\qt_build.ps1` выполнен 2026-07-11 11:41 local time после Scientific Object Editor Phase 1.2: статус **CONFIRMED** для Release + RelWithDebInfo, XR Auto ON. После follow-up world-space overlay/image/localisation fix узкий `RelWithDebInfo gui_client` target пересобран, PubChem apply и molecule-information smoke выполнены из RelWithDebInfo runtime без server/deploy/production. `build` ниже означает запись generated artifacts, даже если source не меняется.

Примечание Phase 2: все существовавшие command blocks сохранены без изменения; документационная миграция не выполняла ни одну из этих команд. Для текущей задачи допустимы только static link/path/term/diff checks.

## Требования окружения

| Требование | Подтверждение / роль |
| --- | --- |
| CMake, Ruby, Git | root configure вызывает Ruby config; dependency/build scripts Ruby |
| C++17 | `cmake/shared_cxx_settings.cmake` |
| `GLARE_CORE_LIBS` | root внешних libraries/source trees |
| `WINTER_DIR` | Winter engine source/install |
| `GLARE_CORE_TRUNK_DIR` | glare-core source |
| `CYBERSPACE_OUTPUT` | runtime/build output root |
| Qt | Windows 5.15.16; macOS/Linux 5.15.10 в `scripts/config-lib.rb` |
| VS2022 x64 | canonical Windows generator/local wrapper |
| SDL2 | alternative native и обязательный Emscripten path |
| LLVM 15, LibreSSL, libjpeg-turbo | root CMake dependencies |
| Jolt 5.3.0, Luau 0.627 | fetched by `scripts/get_libs.rb` into external dependency root |
| Optional CEF | browser/webview helper; local wrapper default off |
| Optional OpenXR SDK | `XR_SUPPORT=ON`; must contain `cmake/OpenXRConfig.cmake` |
| Emscripten/Ninja | webclient build only; separate SDL/libjpeg Emscripten builds |

Current host has `C:\programming\qt_build.ps1`, `substrata_build_qt`, `substrata_output_qt` and OpenXR SDK. `qt6_build.ps1`/`sdl_build.ps1` are absent.

## Быстрые read-only проверки

```powershell
git -C C:\programming\substrata status --short --branch
git -C C:\programming\substrata branch -vv
rg -n "SymbolOrRoute" C:\programming\substrata
python C:\programming\substrata\scripts\analyze_xr_pose_trace.py [optional-trace.csv]
```

Последняя Python-команда только читает trace и печатает анализ. Для Markdown-задачи минимальная проверка — relative links/path existence + `git diff --check`; C++ build не нужен.

## Windows native: канонический local wrapper

Wrapper находится вне Git repo и проверен чтением на audit host.

Статус последнего подтверждения: **CONFIRMED, 2026-07-11**.

Подтверждённая команда из PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1
```

Эквивалент при прямом запуске из PowerShell:

```powershell
& "C:\programming\qt_build.ps1"
```

Фактически подтверждённый запуск выполнялся из `C:\programming`; рабочий каталог не является contract, потому что script использует абсолютные defaults.

Подтверждённые параметры/результаты из `qt_build.ps1` и `C:\programming\substrata_output_qt\build_manifest.json`:

| Поле | Значение |
| --- | --- |
| Source dir | `C:\programming\substrata` |
| Build dir | `C:\programming\substrata_build_qt` |
| Output root | `C:\programming\substrata_output_qt` |
| Generator | Visual Studio 17 2022 x64 |
| Target | `gui_client` |
| Configs | `Release`, `RelWithDebInfo` |
| Jobs | 8 |
| Qt | 5.15.16 at `C:/programming/Qt/5.15.16-vs2022-64` |
| CEF | OFF |
| XR | Auto; on 2026-07-11 resolved to `XR_SUPPORT=ON` with `C:\programming\OpenXR-SDK-1.1.57\install` |
| Runtime copy | enabled; wrapper validates required Qt/platform artifacts |
| Manifest | `C:\programming\substrata_output_qt\build_manifest.json` |

Confirmed client artifacts:

```text
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\Release\gui_client.exe
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe
```

Подтверждение build относится к compile/link/runtime-copy artifact validation. Для PubChem есть отдельный narrow runtime smoke ниже; он не заменяет manual editor UI/server/reconnect test.

### Scientific PubChem HTTPS smoke

Статус: **CONFIRMED, 2026-07-11** for headless client provider smoke only.

Запускать из output config directory или с абсолютным путём к report:

```powershell
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe --scientific_pubchem_smoke C:\Users\densh\AppData\Local\Temp\metasiberia_pubchem_smoke.json
```

Фактически подтверждено 2026-07-11:

- transport: Windows WinHTTP/SChannel, not QtNetwork SSL;
- QtNetwork SSL in local Qt build: disabled by Qt config, so `QNetworkAccessManager` must not be used for PubChem HTTPS;
- TLS certificate validation: Windows default validation; certificate errors are not ignored and HTTPS is not downgraded to HTTP;
- `water`: search HTTP 200, CID 962, properties HTTP 200, SDF 3D HTTP 200, PNG HTTP 200;
- `nicotine`: search HTTP 200, CID 89594, properties HTTP 200, SDF 3D HTTP 200, PNG HTTP 200;
- invalid query: HTTP 404/no CID, no built-in sample fallback.

Side effects: writes normal client appdata log/cache and the requested JSON report. It exits before normal server connection/UI flow and is not a general test suite.

### Scientific PubChem object-application smoke

Статус: **CONFIRMED, 2026-07-11** for QWidget editor slot flow and local `WorldObject` application only.

Запускать из output config directory или с абсолютным путём к report:

```powershell
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe --scientific_pubchem_apply_smoke C:\Users\densh\AppData\Local\Temp\metasiberia_pubchem_apply_smoke.json
```

Фактически подтверждено 2026-07-11:

- `water`: one result, selected CID 962, atom count 3, bond count 2, `source=PubChem`, lazy Images load/cache succeeds, local molecule OBJ assigned;
- `nicotine`: one result, selected CID 89594, atom count 26, bond count 27, `source=PubChem`, lazy Images load/cache succeeds, local molecule OBJ assigned;
- labels and legend are enabled and rendered for both molecules; PubChem PNG preview/cache succeeds; lazy PUG View Classification succeeds; serialized marker sizes are 7,616 bytes for Water and 9,098 bytes for Nicotine, below the 10,000-byte shared limit;
- invalid query: HTTP 404/no CID, object source stayed `manual`, no built-in sample fallback.

Scope boundary: this smoke instantiates the Qt widget and calls the same search/load slots, then calls `toObject()` on a local test `WorldObject`. It does **not** connect to a server, create/update production world state, run `GUIClient::objectEdited()` resource conversion/upload, or verify reconnect/second-client persistence.

### Scientific molecule information-layer smoke

Статус: **CONFIRMED, 2026-07-11** for native Qt information-layer logic and live resolver/cache requests.

```powershell
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe --scientific_molecule_info_smoke C:\Users\densh\AppData\Local\Temp\metasiberia_molecule_info_smoke.json
```

Подтверждено:

- exact-first Russian resolver returns `вода` 962, `никотин` 89594, `аспирин` 2244, `кофеин` 2519, `этанол` 702, `глюкоза` 5793;
- seven label modes render with labels and legend enabled;
- atom, multiple-atoms, bond and molecule selected states round-trip in the native model;
- distance, angle and torsion produce three saved measurement records;
- PUG View Classification lazy load/cache succeeds;
- native periodic model contains 118 records.

Scope boundary: this smoke renders the Qt molecule viewport and tests model/actions; it does not verify human-readable visual layout, real mouse ergonomics, server persistence/reconnect, SDL/Web or production. Do not force `QT_QPA_PLATFORM=offscreen` for this runtime unless the offscreen platform plugin is present; on the current Windows Qt output it makes `QApplication` fail during platform integration.

```powershell
# Узкая актуальная конфигурация
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1 -Configs RelWithDebInfo -XR Off

# Полный default wrapper: Release + RelWithDebInfo, XR Auto, CEF off
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1
```

Класс: medium/expensive, пишет build/output/runtime files и `build_manifest.json`. `-SkipConfigure` допустим только для уже согласованного tree. `-XR On` обязан падать без SDK.

Ручная конфигурация Qt path:

```powershell
$env:GLARE_CORE_LIBS = "C:/programming"
$env:WINTER_DIR = "C:/programming/winter"
$env:GLARE_CORE_TRUNK_DIR = "C:/programming/glare-core"
$env:CYBERSPACE_OUTPUT = "C:/programming/substrata_output_qt"

cmake -S C:\programming\substrata `
  -B C:\programming\substrata_build_qt `
  -G "Visual Studio 17 2022" -A x64 `
  -DUSE_SDL=OFF -DCEF_SUPPORT=OFF -DXR_SUPPORT=OFF
```

## Узкие target builds

```powershell
cmake --build C:\programming\substrata_build_qt --config RelWithDebInfo --target gui_client -j 8
cmake --build C:\programming\substrata_build_qt --config RelWithDebInfo --target server -j 8
cmake --build C:\programming\substrata_build_qt --config RelWithDebInfo --target screenshot_bot -j 8
```

- Fast/medium incremental, expensive clean; writes artifacts.
- Single-file standard command отсутствует; incremental affected target — минимальная compile check.
- Full root build: `cmake --build C:\programming\substrata_build_qt --config RelWithDebInfo -j 8`; включает root-wired targets и conditional CEF helper.

Runtime staging из `scripts/`:

```powershell
ruby copy_files_to_output.rb --no_bugsplat --no_cef --config RelWithDebInfo
```

Команда пишет runtime output. Запускать только с корректным `CYBERSPACE_OUTPUT`.

## Встроенные tests

CTest/add_test в repo нет. `BUILD_TESTS=1` автоматически задаётся только Debug и RelWithDebInfo.

```powershell
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe --test
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\server.exe --test
```

- Release `--test` не поддерживает compiled test suite.
- Suites не полностью hermetic: включают file/codec/model/database, TLS/HTTP/SMTP-related tests. Перед запуском оценить network/local-state side effects.
- `WebServerRequestHandlerTests`, `WorkerThreadTests` и некоторые terrain tests существуют, но отдельный wired runner не подтверждён.
- Debug полезен для leak detection, но дороже.

## Linux configure/build

CI подтверждает только configure shape:

```bash
cmake -S . -B build \
  -DCEF_SUPPORT=OFF \
  -DUSE_SDL=ON \
  -DSDL_BUILD_DIR=/usr \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --target server -j 4
```

Требуются корректные external dependency trees/env. CI не выполняет вторую строку и не подтверждает link/runtime.

## Компонентная матрица проверок

| Компонент | Минимум | Локальная сборка/test | Интеграция | Ограничения |
| --- | --- | --- | --- | --- |
| Qt client/UI/editor | affected symbol/CMake list | `gui_client` target + `--test` | manual UI flow; server при protocol | default client run пишет appdata/может подключаться к production |
| Scientific Object Editor WIP | marker/schema/CMake/10 KB bound | тот же `gui_client` target после отдельного разрешения | create/edit/reconnect/resource/permission flow | current implementation Qt-based, untracked WIP; Web/SDL parity и adapters/AI execution не считать реализованными |
| SDL client | compile guards/search | configure `USE_SDL=ON`, build `gui_client` | manual SDL flow | отдельный current wrapper отсутствует |
| XR | CMake/guard + target | wrapper `-XR On/Off`, client test | `--desktop` и `--vr`, HMD/runtime | runtime/HMD external; не запускать как generic smoke |
| Server | affected target | `server` + `--test` | local copy state + client; route smoke | обычный run меняет state/listens ports |
| Embedded web/admin/API | route/handler/assets | build/test `server` | localhost HTTP/auth/permission flow | отдельного target/server нет |
| Shared protocol/model | readers/writers matrix | client + server builds/tests | old fixture/cross-version peers | architecture-level risk |
| Screenshot pipeline | target build | `screenshot_bot` build | isolated server + GUI slave | production/default run external и long-running |
| Webclient | HTML/script read | Emscripten `gui_client` | local server `/webclient` + WSS | expensive toolchain; generated outputs |
| Site CSS/JS/fragments | link/path/syntax read | server build only if C++ changed | local WebDataStore/site route | publish/deploy separate |
| Node site capture | `package.json`/script | `npm ci`; explicit capture | local/non-sensitive URL | network/output; auth state sensitive |
| Optional bots/installer | source/CMake audit | command не подтверждён | none | root-disabled/stale dependencies |

## Emscripten/webclient

Source command shape из `docs/building.txt`:

```powershell
cmake -S C:\programming\substrata -B <isolated-emscripten-build> `
  -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=<emsdk>/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake `
  -DCMAKE_BUILD_TYPE=RelWithDebInfo -DUSE_SDL=ON -DCEF_SUPPORT=OFF `
  -DSDL_BUILD_DIR=<emscripten-sdl-build>

Set-Location <isolated-emscripten-build>
ruby C:\programming\substrata\scripts\make_emscripten_preload_data.rb C:\programming\substrata
cmake --build . --target gui_client
ruby C:\programming\substrata\scripts\update_webclient_cache_busting_hashes.rb C:\programming\substrata
```

Класс: expensive/data-changing. Важно: preload script удаляет/создаёт `./data`, пишет в output и touch-ит внешний `GLARE_CORE_TRUNK_DIR/graphics/TextureData.cpp`; запускать только из изолированного build dir. Tracked `emscripten_build*` не считать portable source.

## Site/Figma и media helpers

```powershell
Set-Location C:\programming\substrata\tools\site_capture
npm ci
node capture.mjs --base http://127.0.0.1:<port> --out <safe-output-dir>
```

- Medium/network/output-writing. Для authenticated pages capture может сохранить sensitive `storageState.json`, cookies и screenshots.
- `capture_site_for_figma.ps1` может выполнить `npm install` автоматически.
- `snapshot_site_pages.ps1` пишет HTML/assets/manifest.
- VIVE sync — long-running ADB media pull; task-only, private output.

## Lint, format и type checks

- Repo-level clang-format/clang-tidy/editorconfig/ESLint/Prettier/typecheck config не найден.
- Не придумывать «каноническую» formatting command и не форматировать unrelated code.
- Ruby syntax: `ruby -c <script.rb>`; Python syntax/import-safe check и PowerShell parser допустимы только если не выполняют side effects.
- C++ correctness минимум подтверждает affected target compile, не text-only checker.

## CI: что реально проверяется

- `.github/workflows/windows-main-ci.yml` и `cmake-configure-linux.yml` выполняют checkout dependencies, Jolt/Luau fetch и CMake configure.
- Compile, tests, lint, packaging и deploy отсутствуют.
- Windows создаёт dummy SDL header только для configure; real headers/link не проверены.
- `webserver/**`, `screenshot_bot/**`, assets и systemd отсутствуют в path filters, хотя webserver входит в server.
- External `glare-core`/`winter` refs не pinned; configure не полностью reproducible.

## Опасные и publishing команды

| Команда/flow | Класс | Почему не запускать без явного задания |
| --- | --- | --- |
| `server` без `--test` | dangerous/data-changing | создаёт/читает/пишет state, directories, listeners |
| `server --compact_database_in_place` | destructive | меняет binary DB in-place |
| `gui_client --desktop/--vr` | external/data-changing | default production URL, appdata/cache/settings/logs |
| screenshot/lightmapper/stress bots | external/long-running | подключение, upload/render/load на server |
| production release symlink + `systemctl restart` | deploy | переключает live binary/service |
| `deploy_web_to_metasiberia_v2.ps1` | remote publish/destructive | old host/paths, `rsync --delete` |
| `publish_update.ps1` | publish | version/commit/tag/push/build/release; historical remote/policy |
| `publish_wiki_to_github.ps1` | publish | clone/delete/commit/push Wiki |
| `metasiberia_backup_pull.ps1` | sensitive/data-changing | SSH/SCP archives и retention deletion |
| map maintenance `sample/regen` | production/sensitive | читает/caches admin session; regen меняет queue/state |

Production workflow допустимо **описать**, но build/deploy/restart/DNS/restore выполнять только после отдельного подтверждения пользователя.

## Неподтверждённые команды/границы

- Disabled `cv_bot`, `lightmapper_bot`, `backup_bot`, `stress_test` не имеют current root build command без изменения build graph.
- `installer/CMakeLists.txt` root integration отсутствует.
- Staging `server_dist_resources/` → runtime `dist_resources/` не найден.
- Public `metasiberia.com`, TheRift и Android wrapper не имеют build source/command в repo.
- Fuzzing требует manual source/build-property changes и не является turnkey suite.

## Scientific Object Editor regression smoke

`gui_client.exe --scientific_molecule_info_smoke <report.json>` теперь дополнительно проверяет replacement path: создать distance/angle/torsion measurements, заменить molecule structure и убедиться, что stale atom-index-dependent measurements очищены (`stale_measurements_cleared_after_molecule_replace=true`). Это покрывает crash path, найденный 2026-07-11 при загрузке новой PubChem molecule в объект.
