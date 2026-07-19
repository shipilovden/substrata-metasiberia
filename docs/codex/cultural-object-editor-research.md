# Техническое исследование: «Редактор объектов культуры»

Проверено: 2026-07-17. Это архитектурное исследование, а не доказательство готовности функций. Первый client-side MVP появился в active working tree 2026-07-18; его фактические границы и незавершённые части зафиксированы в [native-editors-stage-status-2026-07-18.md](native-editors-stage-status-2026-07-18.md). Runtime и production из этого документа не подтверждаются.

## Итог

Редактор реализуем в текущей архитектуре без изменения дизайна клиента. Наиболее безопасный первый вариант — Qt-панель, открываемая тем же `MainWindow::showObjectEditor()` и работающая с `WorldObject::ObjectType_Generic`. Специализированные данные следует хранить в ограниченном versioned envelope, а большие JSON/API-ответы, изображения, IIIF, 3D, аудио и видео — как content-addressed resources через `ResourceManager`. Это повторяет проверенную границу Scientific WIP и не требует немедленного нового protocol message или серверного типа.

Полноценная музейная федерация, entity resolution, provenance и лицензии — отдельная подсистема. Нельзя делать её набором полей Qt-формы или прямыми вызовами музейных API из виджета. Нужен промежуточный `Cultural API`/provider layer, сначала локальный/клиентский, затем серверный кэш при согласовании протокола и production deployment.

Сложность: высокая (ориентировочно 7–10 инженерных этапов, 3 независимых риска: bounded object payload, медиалицензии, неоднородность API). Минимальный полезный прототип: `CulturalObject` envelope + локальный JSON/IIIF import + карточка/подпись + generic object create/update + offline fixtures. Первый online provider — Art Institute of Chicago или The Met; не начинать с десяти адаптеров.

## 1. Что найдено в проекте

| Возможность | Подтверждённый код/документ | Вывод |
|---|---|---|
| Специализированный Qt editor | `gui_client/ScientificObjectEditor.*`, `ScientificObjectSettings.*` | переиспользовать lifecycle, сигналы и форму панели, но отдельная модель `CulturalObject` |
| Выбор редактора | `MainWindow::setObjectEditorFromOb`, `objectEditorToObject`, `showObjectEditor` | добавить `ActiveEditor_Cultural`, discriminator и ветку показа; левый dock останется тем же |
| Создание | `MainWindow::on_actionAddScientificObject_triggered()` | скопировать только generic create/permissions/confirmation flow; создать отдельный `on_actionAddCulturalObject_triggered()` |
| World entity | `shared/WorldObject.*` | authoritative identity — server `WorldObject::uid`; `uuid` культурной записи — внешний стабильный идентификатор |
| Serialization | `WorldObject::writeToNetworkStream`, `readWorldObjectFromNetworkStreamGivenUID` | `object_type`, transform, materials, URL, `script`, `content` уже сериализуются |
| Специализированный envelope | marker первой строки + JSON в `WorldObject::content` | совместимо с generic clients, но `MAX_CONTENT_SIZE = 10000` bytes — жёсткий предел |
| Server create/update | `server/WorkerThread.cpp`: `Protocol::CreateObject`, `ObjectFullUpdate` | сервер сохраняет generic content и проверяет permissions/resources; Cultural JSON v1 не должен требовать нового ID |
| Resources | `shared/ResourceManager.*`, `gui_client/DownloadResourcesThread.*`, `NetDownloadResourcesThread.*` | использовать checksum URL и обычный upload/download; bulk не класть в `content` |
| Images/video/audio | `ScientificImageViewer.*`, `BrowserVidPlayer.*`, `AddVideoDialog.*`, `LoadAudioTask.*` | переиспользовать загрузчики/просмотрщики как адаптеры, не копировать музейную бизнес-логику |
| Cards/nametags | generic `WorldObject` metadata, `ObjectType_Text`, object/selection UI | карточку хранить в Cultural settings; постоянную отдельную табличку делать только после отдельного child-object contract |
| JSON | Qt `QJsonDocument`/`QJsonObject` в существующих виджетах; Scientific serializer | использовать строгую версию схемы, unknown-field preservation и диагностику |
| HTTP | существующие resource/network paths; Scientific PubChem transport имеет Windows SSL caveat | provider requests должны быть вне render loop, с timeout/cancel/cache; лучше общий `CulturalApiClient` |

Scientific editor — WIP: PubChem является первым подтверждённым онлайн-провайдером, остальные пункты его UI не равны готовым adapters. Его известные долги (10 KB, opaque server schema, отсутствие migration framework) прямо относятся к Cultural editor и должны быть исправлены архитектурно, а не замаскированы расширением формы.

## 2. Предлагаемая архитектура

```text
Qt CulturalObjectEditorWidget (left dock)
  -> CulturalEditorController (commands, validation, dirty state)
  -> CulturalApiClient (async, cache, rate/cancel)
  -> CulturalProviderRegistry
       -> AICProvider / MetProvider / SmithsonianProvider / ...
       -> EuropeanaProvider / WikidataProvider / IIIFProvider
  -> CulturalNormalizer + EntityResolver + MergePlanner
  -> CulturalObjectRepository (local cache/resources)
  -> WorldObjectAdapter (bounded envelope + model/material/URL)
  -> existing GUIClient objectEdited/CreateObject/ObjectFullUpdate
```

Провайдер не должен знать Qt widgets. `CulturalApiClient` не должен знать музейные поля; adapter переводит ответ в `CulturalSourceRecord`. Нормализатор строит варианты полей с provenance. MergePlanner только предлагает объединение; решение low-confidence принимает пользователь.

### Переиспользовать / реализовать отдельно

Переиспользовать: dock и active-editor lifecycle MainWindow; permission/create/update; `WorldObject` transform/material/physics; `ResourceManager`; existing preview/image/video/audio widgets; Qt signals, settings, theme and Lucide SVG icons; generic server persistence.

Отдельно: `CulturalObjectSettings` (не ScientificObjectSettings); provider registry/adapters; normalized typed model; provenance/rights; entity resolution; field conflict UI; cache schema/migrations; cultural tabs; JSON-LD export; optional server Cultural API/cache.

## 3. Модель данных

Модель должна быть typed, но допускает расширение словарями. Строка — только display value, не идентификатор классификатора.

```cpp
struct CulturalFieldValue {
  std::string value, original_value, language;
  std::string source_id, source_name, source_record_id, source_url;
  std::string license_id, license_url, retrieved_at;
  double confidence = 0; bool verified = false, selected_as_primary = false;
  enum Origin { Imported, Normalized, Computed, User } origin;
};
struct CulturalIdentifier { std::string scheme, value, uri; bool preferred; };
struct CulturalCreator { std::string name, role, ulan_id, wikidata_id; std::vector<CulturalFieldValue> names; };
struct CulturalDate { std::string display, from, to, calendar, certainty; };
struct CulturalDimensions { double width=0, height=0, depth=0, weight=0; std::string unit, raw; };
struct CulturalClassification { std::string scheme, id, label, parent_id; double confidence; };
struct CulturalMediaAsset {
  std::string kind, uri, local_resource_url, mime, checksum, iiif_manifest;
  CulturalLicense license; bool primary=false, downloadable=false;
};
struct CulturalLicense {
  enum Status { FreeUse, AttributionRequired, NonCommercial, Restricted,
                MetadataOnly, Unknown, Blocked } status;
  std::string rights_holder, license, license_url, attribution, checked_at;
  bool display=false, download=false, modify=false, commercial=false;
};
struct CulturalSourceRecord {
  std::string provider_id, source_name, record_id, canonical_url, raw_cache_ref,
              retrieved_at, updated_at, raw_checksum, license_summary;
  std::vector<CulturalIdentifier> identifiers;
  enum MatchStatus { Exact, Probable, Possible, None, Conflict, ManualReview } match;
};
struct CulturalProvenanceEvent { std::string type, date, place, actor, description, source_record_id; };
struct CulturalExhibitionEvent { std::string title, venue, from, to, source_record_id; };
struct CulturalRelation { std::string predicate, target_uuid, target_uri, label; };
struct CulturalDisplaySettings {
  std::string title, subtitle, short_description, language, theme, icon;
  std::vector<std::string> visible_fields; bool auto_open=false, open_on_click=true, pin=false;
  std::string plaque_text; float card_scale=1.f;
};
struct CulturalExhibitionSettings {
  std::string scene, room, zone, placement, frame, pedestal, case_style;
  Vec3d position; Vec3f rotation; float scale=1.f, light_intensity=1.f, activation_distance=3.f;
  bool spotlight=false, shadows=true, interactive=false, visible=true, audio_guide=false;
  std::string route_id, curator_note, show_from, show_to;
};
```

`CulturalObjectSettings` envelope:

```text
metasiberia_cultural_object_v1\n
{"schema_version":1,"uuid":"...","object_type":"archaeological_object",
 "categories":[...],"title":{...field variants...},"creators":[...],
 "dates":[],"classifications":[],"materials":[],"techniques":[],
 "identifiers":[],"source_records":[],"media":[],"provenance":[],
 "exhibitions":[],"display":{},"interaction":{},"exhibition":{},
 "custom_fields":{},"raw_source_refs":[]}
```

The envelope stores descriptors and resource references only. Large raw responses, images, models, audio/video and IIIF manifests live in cache/resource storage with checksum and source record. Unknown root fields are preserved. Every write validates UTF-8, depth/count/byte limits and schema version. A future server-side typed record requires ADR, migration and old-client compatibility.

### Classification

Keep separate multi-valued fields: `object_type`, `cultural_category`, `art_forms`, `museum_classifications`, `disciplines`, `materials`, `techniques`, `cultures`, `periods`, `styles`, `genres`, `functions`, `locations`. First version uses internal IDs plus labels; map to Getty AAT/ULAN/TGN/CONA and Wikidata IDs. CIDOC CRM is an interoperability mapping, not the runtime storage schema; EDM/IIIF are export/media mappings. Getty vocabulary data are openly available under ODC-By; attribution is required. CIDOC CRM is an ISO cultural-heritage ontology; IIIF Image/Presentation 3.0 are the first media interoperability targets.

## 4. Sources and Cultural API

All UI search goes through `ICulturalDataProvider`; no museum-specific URLs in widgets.

Required interface:

```text
search(CulturalSearchQuery) -> CulturalSearchPage
getObject(SourceId) -> CulturalSourceRecord
getImages(SourceId), getMedia(SourceId), getIIIFManifest(SourceId)
getLicense(AssetId), getRelatedObjects(SourceId), getProvenance(SourceId)
getExhibitions(SourceId), getPublications(SourceId), get3DAssets(SourceId)
getRawRecord(SourceId) -> checksum-addressed bytes
```

| Provider | Реально доступно | Ограничения/ключ |
|---|---|---|
| Art Institute of Chicago | REST JSON search/detail, fields, public-domain filter, IIIF Image/Manifest, nightly dumps | anonymous throttling; description and media rights differ; use HTTPS and attribution |
| The Met | public Collection API: object listing/detail/search and open-access/image flags | API is collection metadata, rights still per asset; no assumed 3D/audio |
| Smithsonian Open Access | API metadata/assets through `api.data.gov` | register API key; rights per asset |
| Europeana | Search/Record APIs, EDM, IIIF | free API key for most APIs; preserve provider rights statements |
| Rijksmuseum | Search/Linked Data, OAI-PMH, persistent IDs, data dumps | no key for documented search/OAI; search is collection-oriented and not full archive API |
| V&A | Collections API JSON/CSV, image API/IIIF | terms govern metadata and images; API versioning must be tracked |
| Cleveland CMA | REST JSON, daily dataset, image links, exhibitions; CC0 dataset but images only where `share_license_status=CC0` | metadata CC0 does not imply every image CC0 |
| Harvard Art Museums | REST collection API | API key/account required; owner must provision key outside object data |
| Cooper Hewitt | GraphQL collection API | open access, default rate limit; optional access key |
| Getty | Museum collection IIIF + public SPARQL; Getty AAT/ULAN/TGN/CONA vocabularies | primarily linked data/media/vocabularies, not a single universal object REST API |
| Wikidata | REST/Wikibase API and SPARQL | community-edited; use as corroboration/linking, not museum authority; throttle SPARQL |
| IIIF | Image API 3.0 and Presentation API 3.0 | protocol, not catalogue; manifest rights still apply |

Official API evidence: [AIC](https://api.artic.edu/docs/), [Met](https://metmuseum.github.io/), [Smithsonian](https://www.si.edu/openaccess/faq), [Europeana](https://api.europeana.eu/en), [Rijksmuseum](https://data.rijksmuseum.nl/docs/), [V&A](https://developers.vam.ac.uk/guide/v2/welcome.html), [CMA](https://openaccess-api.clevelandart.org/), [Cooper Hewitt](https://apidocs.cooperhewitt.org/getting-started/), [Harvard](https://tech.hvrd.art/about/), [Getty](https://data.getty.edu/museum/collection/docs/), [Wikidata](https://www.wikidata.org/wiki/Help%3AData_access), [IIIF](https://iiif.io/api/index.html).

Google Arts & Culture is deliberately not a provider: only store an external URL when supplied by a source/user; do not scrape or treat it as an official API.

## 5. Entity resolution and merge

Each provider result remains a separate `CulturalSourceRecord` until a merge decision. Candidate score (0–100): exact museum persistent ID 40; Wikidata ID 25; IIIF manifest 15; inventory/accession number 15; normalized title 10; creator authority ID/name 15; creation date overlap 8; dimensions tolerance 6; collection/institution 6; perceptual image hash 12; material/technique 4. Scores are capped and evidence is recorded; this is not a truth score.

* `exact_match`: authoritative identifier match and no contradiction.
* `probable_match`: >=75 with at least two independent signals.
* `possible_match`: 45–74.
* `no_match`: <45.
* `conflict`: strong identifiers but contradictory title/date/media/rights.
* `manual_review_required`: any merge with conflict or below probable threshold.

Merge UI shows source rows, per-field values, confidence, license and retrieval date. User chooses primary value; alternatives and raw references remain. “Разделить записи” reverses a merge by creating independent provenance groups; it never deletes source history. Never auto-merge low-confidence results.

## 6. UI in existing left panel

Do not redesign the client. Add one toolbar/menu action with the project’s existing SVG icon and tooltip «Добавить объект культуры». The same editor dock contains six tabs:

1. **Настройки** — name/type/description, UUID and UID read-only, source/local file/URL/provider, search/import/update/merge/raw/license actions, dirty/error banner.
2. **Данные** — title variants, creators, date, place/culture/period, collection/museum/accession, independent classifications, materials/techniques/style/genre/keywords, identifiers, per-field source/conflict badges and “choose primary”.
3. **Карточка объекта** — thumbnail/preview, title/subtitle/summary, visible fields, theme/style/size/icon/language, click/auto-open/pin and plaque text.
4. **Медиа** — primary/large/additional images, IIIF manifest, 3D, audio/video/documents, gallery, license per asset, cache/lazy-load/open/replace/remove/refresh.
5. **История** — chronology, provenance/owners/transfers, exhibitions, restorations/condition, publications/archive documents and source per event.
6. **Экспозиция** — scene/room/zone, transform, frame/pedestal/case/plaque, light/spotlight/shadows, interaction distance/audio/zoom, route/visibility/date/curator note.

All buttons use existing SVG icon utilities and existing theme palette. Every network control is asynchronous and has disabled/loading/error states; no network call from paint/render loop.

### Button contract

| Кнопки | Активна когда | Ошибки/подтверждение |
|---|---|---|
| Добавить объект культуры | create permission | permission/network; no confirm |
| Найти/выбрать источник | query/provider valid | timeout, 401, 429, malformed; no destructive confirm |
| Импортировать/обновить | selected record and license allows metadata | restricted media warning; confirm before replacing local edits |
| Синхронизировать | source record exists | conflict/offline; show diff, never overwrite silently |
| Объединить/разделить/сравнить/конфликты | >=2 records / merged record | manual selection; merge confirmation |
| Основное значение | field has variants | immediate, undoable |
| Raw/JSON/JSON-LD/source site/IIIF | corresponding URL/cache exists | browser/open errors; no confirm |
| Проверить лицензию | asset selected | unknown rights is warning, not success |
| Load/replace/remove media | local file or URL valid | size/type/license errors; remove confirm |
| Add source/relation/event/exhibition/restoration/owner/publication/translation | editor editable | validation; no confirm |
| Auto-classify/check duplicates | normalized data present | mark computed, never authoritative; no confirm |
| Save/Cancel/reset/export | dirty state | cancel/reset/remove confirm; save validates payload/permissions |

## 7. Flows

**Create:** button → permission check → generic unit-cube Cultural envelope → `CreateObject` → server UID → select object → Cultural tab. No local fake museum data.

**Search:** UI query → CulturalApiClient cache lookup → provider fan-out with per-provider timeout → normalized result cards (thumbnail/title/date/museum/license/IIIF/3D/match) → user selects record → fetch detail/assets on demand.

**Merge:** fetch source records → candidate scoring → comparison grid → user selects primary values → preserve all alternatives/raw records → write bounded envelope and resource refs → ordinary ObjectFullUpdate.

**Refresh:** retain user-origin values; fetch current source; produce diff/conflict list; user accepts fields individually; update cache timestamps and checksums.

**Export:** JSON envelope, JSON-LD mapping (EDM/CIDOC CRM context), raw references and a license/attribution report. Export must never imply that a restricted image is redistributable.

## 8. Cache, licensing, media

Cache key = provider + endpoint + normalized query + API version; object asset key = provider record ID + representation + checksum. Store raw response, normalized record, thumbnail, IIIF manifest, media and license metadata separately with retrieved/expiry timestamps. Cache is local first; server shared cache is a later ADR. Never overwrite user-edited fields on refresh.

Metadata rights and media rights are independent. Asset status is `free_use`, `attribution_required`, `non_commercial`, `restricted`, `metadata_only`, `unknown_license`, `blocked`. Import of `restricted`/`unknown` media requires explicit warning and can store URL-only metadata. Record rights holder, license URL, display/download/modify/commercial flags, attribution text and check date per asset.

## 9. Network and persistence plan

**v1:** no new protocol IDs. `WorldObject::ObjectType_Generic` + `metasiberia_cultural_object_v1` marker; server treats envelope as opaque, just as Scientific WIP. Large data uses existing resource dependency flow. This preserves old clients (they see a generic object) and avoids production schema migration.

**v2 (only after prototype):** if cross-client search, ACL, shared cache or large provenance queries are required, introduce server `Cultural API` endpoints and a bounded `CulturalObjectDescriptor` shared contract. Require ADR, protocol version, permissions, migration, server Linux build/deploy and old-client behavior tests. Do not put API keys in object content.

## 10. Text diagrams

```text
User -> left dock -> Controller -> Cultural API -> Provider adapters -> museum/IIIF
                                      |-> cache/raw/license
                                      |-> normalizer -> resolver -> merge planner
Controller -> WorldObjectAdapter -> CreateObject/ObjectFullUpdate -> Linux server/state
```

```text
source records --identifiers/title/creator/date/media--> candidates
       -> score -> exact/probable/possible/conflict
       -> user field selection -> CulturalFieldValue[] + primary flag
       -> bounded envelope + content-addressed resources
```

```text
CulturalObject
  identity + classifications
  field variants -> provenance
  source records -> raw/cache refs
  media -> rights + resource refs
  history/relations
  display/exhibition (derivative world presentation)
```

Lifecycle: `draft → source_search → imported → normalized → review_required → merged → placed → published → refreshed → archived`; any provider failure branches to `offline/error` without mutating the last valid snapshot.

## 11. Файлы и классы

Первый implementation slice:

* `gui_client/CulturalObjectSettings.{h,cpp}` — schema, limits, migration/defaults, JSON round-trip.
* `gui_client/CulturalObjectEditor.{h,cpp}` — six tabs and signals; narrow UI logic.
* `gui_client/CulturalApiClient.{h,cpp}`, `CulturalProviderRegistry.*`, `ICulturalDataProvider.h` — async transport/cache boundary.
* `gui_client/CulturalObjectNormalizer.*`, `CulturalEntityResolver.*`, `CulturalMergeModel.*` — pure/testable logic.
* `gui_client/MainWindow.{h,cpp}` — action, active-editor enum, selection/show/toObject wiring.
* `gui_client/CMakeLists.txt`, `.ui`/SVG icon registration — build assets only.
* `shared/WorldObject.*` — initially no change; later only via ADR for typed/shared descriptor.
* `server/WorkerThread.cpp` — initially no change; later API/cache endpoints and validation only after protocol design.
* `docs/codex/data-map.md`, `architecture.md`, `documentation-index.md`, `engineering-debt.md` — update when implementation starts.

## 12. Поэтапный план

| Этап | Состав | Tests/готовность | Риски |
|---|---|---|---|
| 0 research | ADR, schema, rights/provider matrix | fixture and compatibility plan | wrong API assumptions |
| 1 local MVP | envelope, create/open, Settings/Data, local JSON/IIIF URL, card/plaque | round-trip, malformed/oversize, generic object regression | 10 KB bound |
| 2 first provider | AIC or Met, async search/detail, attribution, thumbnail | known/unknown/offline/429, cache hit | provider terms/rate |
| 3 media | IIIF Image/Presentation, ResourceManager, lazy gallery, license gate | checksum, no-license import, reconnect | large assets |
| 4 federation | 3–4 providers + Wikidata linking, normalized classifications | provider contract fixtures | schema heterogeneity |
| 5 resolution/merge | scoring, conflict grid, provenance, split/undo | deterministic scores and manual merge | false positives |
| 6 history/exhibition | provenance/events/publications and exhibition settings | serialization and permission tests | model bloat |
| 7 server Cultural API | shared cache/search/ACL only if required | Linux server build/deploy, compatibility, load tests | protocol/state migration |
| 8 export | JSON-LD, EDM/CIDOC mapping, Getty IDs | golden exports and rights report | semantic loss |

## 13. Risks and acceptance criteria

Major risks: `content` overflow; raw source/licence loss; provider API/key/rate changes; image metadata vs image rights confusion; false duplicate merge; UI blocking on network; resource URL availability after reconnect; generic clients not displaying labels; server/client protocol drift; uncontrolled raw JSON/PII.

Before calling the editor ready: no source fallback to demo data; local fixtures pass; malformed/unknown schema preserved; payload guard tested; AIC/Met provider has real search/detail/license/error paths; media checksum and rights warning work; merge is reversible; second-client/reconnect sees the object; old client still renders generic object; no keys in content/logs; docs and ADR updated.

## Вывод

Да, реализовать возможно, и существующий Scientific Object Editor даёт подходящий lifecycle/UI precedent. Фундаментально необходимы не новый внешний дизайн и не копирование `ScientificObject`, а отдельные typed cultural models, provider/Cultural API boundary, provenance-aware merge, rights model и bounded-resource persistence. Начинать следует с локального MVP и одного официального provider, затем доказать media/rights/reconnect, и только после этого расширять федерацию и серверный API.
