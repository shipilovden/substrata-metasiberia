# Статус первого этапа native-редакторов и интеграций

Проверено по исходному коду active working tree: 2026-07-18. Этот документ отделяет уже внесённый каркас от последующих этапов. Он не является отчётом о сборке, ручной проверке клиента или production deployment.

## Границы этапа

Текущий этап добавляет native Qt-поверхности без изменения существующего сетевого протокола, `shared/**`, `server/**` и формата server state. Новые специализированные записи пока остаются generic `WorldObject` с versioned marker в `WorldObject::content`. Большие данные и медиа не должны помещаться в этот envelope.

| Подсистема | Реализованный каркас | Что ещё не реализовано |
| --- | --- | --- |
| Редактор объектов культуры | `CulturalObjectSettings.*`, `CulturalObjectEditor.*`, действие «Добавить объект культуры», discriminator и lifecycle в общем левом editor dock; шесть вкладок; локальный JSON import/raw preview; базовые license/media/exhibition/transform поля | Cultural API, реальные museum providers, асинхронный поиск, кэш, entity resolution, merge/conflict UI, provenance каждого значения, импорт ресурсов и JSON-LD |
| Редактор анимаций | `AnimationEditorPanel.*` в масштабируемом правом `QDockWidget`; библиотека/поиск, transport UI, вкладки настроек, назначений, переходов, событий, скелета и импорта; профили и undo/redo через `QSettings` | полноценное применение набора к runtime animation graph, retargeting, валидация импортированных клипов и серверное распространение наборов |
| Фото/видео | `PhotoVideoSettingsPanel.*` в правом `QDockWidget`; camera/video/output tabs, пресеты `QSettings`, параметры оптики/кадра, существующий screenshot/gallery flow | backend видеозаписи/кодирования, захват системного звука, проверка codec/container combinations и публикация результата как ресурса |
| Документы | `DocumentObjectSettings.*`, `DocumentEditorPanel.*`, правый document dock и действие «Добавить документ»; TXT/HTML/Markdown, `QTextDocument` preview/edit, поиск, сохранение/экспорт и bounded descriptor | создание экранного объекта в мире, upload/content-addressed resource flow, привязка к модели/Scientific/Cultural object и синхронизация descriptor с server object |
| MCP | настройки disabled-by-default (`mcp_client/enabled`, порт 8095) и `MCPClientHandler.*`: проверка loopback peer/path/method/body/JSON, `render_view` callback и HTTPS forwarding с сохранёнными credentials | безопасный listener не запускается: текущий общий `WebListenerThread` слушает wildcard; нужен API явного bind на `127.0.0.1`/`::1`, lifecycle start/stop и integration tests |

## Cultural Object MVP

`CulturalObjectSettings` использует marker `metasiberia_cultural_object_v1` и `schema_version = 1`. В envelope находятся только редактируемые descriptor-поля и ссылки: идентичность, классификации, карточка, медиа URL, права, история и параметры экспозиции. Исходные музейные ответы, изображения, IIIF manifests, 3D, аудио и видео должны храниться отдельно и адресоваться ссылкой/checksum.

Редактор встроен в существующую левую панель и повторяет lifecycle Scientific editor, но не использует `ScientificObjectSettings`. Вкладки первого этапа: «Настройки», «Данные», «Карточка объекта», «Медиа», «История», «Экспозиция». Создание использует `WorldObject::ObjectType_Generic`, поэтому текущий этап не требует нового protocol message или Linux-server deployment.

Кнопки online search/import/merge нельзя объявлять готовыми до появления `ICulturalDataProvider`/`CulturalApiClient`. Следующий минимальный vertical slice: offline fixtures -> один provider (AIC или The Met) -> нормализация -> license gate -> resource import -> сохранение provenance.

Полная архитектура и последующие этапы остаются в [cultural-object-editor-research.md](cultural-object-editor-research.md).

## Редактор анимаций

Панель является первым native UI/profile этапом. Профили сохраняются локально через `QSettings`; библиотека и transport публикуют Qt signals. Наличие вкладки или кнопки «Применить» не означает, что уже реализованы animation graph, загрузка клипа в движок, retargeting и сетевое сохранение.

Следующий этап:

1. связать выбранный clip/profile с существующим avatar animation controller;
2. ввести проверяемую модель assignment/transitions/events;
3. валидировать skeleton mapping и import formats;
4. определить границу локального профиля и серверного asset/reference;
5. добавить unit tests сериализации профилей и ручные сценарии preview/apply/reload.

## Фото и видео

Панель отделена от legacy overlay и сохраняет пресеты локально. Параметры камеры должны применяться через существующий camera/photo-mode API; screenshot и gallery переиспользуют существующий клиентский поток. Видеокнопка не должна имитировать запись до подключения encoder/backend.

Следующий этап: выбрать поддерживаемые container/codec, реализовать start/stop/error state, audio sources, atomic output, metadata, resource upload и тесты на отмену/нехватку места/ошибку encoder.

## Документы и Qt PDF

Внутренний формат `metasiberia_document_object_v1` ограничен существующей границей `WorldObject::MAX_CONTENT_SIZE` (10 000 bytes) и хранит metadata/resource references, а не байты документа. TXT, HTML и Markdown обслуживаются native `QTextDocument`/`QTextEdit`.

PDF сделан compile-time capability `METASIBERIA_QT_PDF_AVAILABLE`. Каноническая локальная Qt 5.15.16 установка сейчас не содержит `QtPdf`/`QtPdfWidgets`; кроме того, готовый `QPdfView` относится к Qt 6 API. Поэтому PDF UI должен честно показывать отсутствие capability и не подменять его картинкой или внешним браузером.

Безопасные варианты продолжения:

- добавить совместимый Qt PDF module для текущего toolchain и реализовать собственный Qt 5 viewer вокруг `QPdfDocument`;
- либо завершить контролируемую Qt 6 migration и использовать `QPdfView`;
- до выбора зависимости оставить PDF feature guarded и не менять canonical Qt build скрытно.

После появления viewer нужны поиск/selection/internal links/history/outline/bookmarks/rotate/page-to-image, затем world-screen object и resource pipeline.

## MCP: обязательная security boundary

Настройки и request handler являются подготовительным портом upstream-функции. Endpoint по умолчанию выключен; пароль не показывается и не сохраняется в MCP settings. Проверка remote peer внутри handler недостаточна: socket обязан физически bind только loopback.

Текущий `web::WebListenerThread` использует wildcard bind, поэтому подключать к нему MCP handler и запускать порт 8095 нельзя. До готовности явного loopback bind пользовательский флаг должен считаться неактивной подготовительной настройкой, а не работающим сервером.

Критерии готовности MCP listener:

1. bind только `127.0.0.1` и, при отдельной проверке, `::1`;
2. disabled-by-default и предсказуемый start/stop при изменении настройки;
3. строгие ограничения request size, method/path, JSON и image dimensions;
4. отсутствие password/API key в UI, logs и object state;
5. tests с loopback request и доказательством недоступности с LAN-интерфейса;
6. forwarding только на текущий HTTPS server с явной ошибкой при отсутствии credentials.

## Проверка и критерии завершения этапа

Перед объявлением этапа готовым требуется canonical Windows Qt build через `C:\programming\qt_build.ps1`, проверка `build_manifest.json` и canonical `RelWithDebInfo\gui_client.exe`. Сам клиент без прямого запроса владельца не запускать. Поскольку текущий этап не меняет `server/**`, server-side `shared/**` или протокол, Linux production server пересобирать и деплоить не требуется.

Ручная проверка владельцем должна подтвердить: открытие/масштабирование dock panels, сохранение профилей после перезапуска, создание/повторное открытие Cultural generic object, JSON round-trip и 10 KB rejection, честное PDF-unavailable состояние и отсутствие слушающего MCP-порта при выключенной/неподдержанной интеграции.
