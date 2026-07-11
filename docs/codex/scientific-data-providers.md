# Scientific Data Providers

Назначение: каноническое описание внешних научных источников, API, форматов, provider-архитектуры и правил загрузки данных для будущего развития Scientific Object Editor.

Проверено: 2026-07-10 по официальным страницам источников и текущей документации MetaSiberia. Этот документ **не доказывает**, что adapters уже реализованы в коде. Реальное состояние текущего Qt WIP фиксируется в [scientific-object-editor.md](scientific-object-editor.md).

## Роль документа

Этот документ нужен, чтобы будущие задачи не повторяли длинный поиск по PubChem/RCSB/ChEBI/NCBI/EMDB/Materials sources и не создавали фальшивые интеграции.

Он является:

- архитектурным справочником для будущего provider layer;
- картой официальных API и форматов;
- правилом сохранения provenance;
- маршрутом для выбора первого real adapter/importer.

Он не является:

- подтверждением runtime-интеграции;
- списком уже работающих provider classes;
- заменой официальной документации источника;
- разрешением хранить API keys в object content;
- научной лицензией на повторное распространение данных.

При конфликте:

1. рабочий код и фактический runtime behavior;
2. официальная документация источника данных;
3. этот документ;
4. prompts/tasks.

## Общее правило научной достоверности

Scientific Object Editor не должен подменять неизвестные данные демонстрационным объектом.

Запрещено:

- заменять failed/unsupported query молекулой Caffeine, Water, Nicotine или любым sample object;
- показывать provider result, если данные взяты из built-in sample;
- придумывать атомы, связи, последовательности, crystal cell, density maps или material properties;
- смешивать импортированные, вычисленные и пользовательские данные без явного source marker;
- терять исходный source identifier;
- сохранять bulk scientific data напрямую в bounded `WorldObject::content`.

Если provider не реализован, UI должен показывать `Unsupported` или `Error` и сохранять старые научные данные без мутации.

## Целевая provider architecture

Provider layer должен быть registry-based, а не большим `switch` по всем дисциплинам.

```text
ScientificProviderRegistry
  -> ScientificDataProvider
       -> PubChemProvider
       -> RcsbProvider
       -> ChebiProvider
       -> AlphaFoldProvider
       -> NcbiProvider
       -> CodProvider
       -> MaterialsProjectProvider
       -> OqmdProvider
       -> EmdbProvider
       -> UniProtProvider
       -> ChemSpiderProvider
       -> LocalFileProvider
```

Минимальные interface roles:

| Interface/descriptor | Ответственность |
| --- | --- |
| `ScientificProviderDescriptor` | id, display name, supported object types, supported query kinds, required credentials, license warning, rate policy |
| `ScientificDataProvider` | search/fetch metadata/fetch raw asset, error states, cancellation, rate limiting |
| `ScientificSearchQuery` | type, query text, identifier kind, source filters, max results |
| `ScientificSearchResult` | provider id, canonical source id, title/name, summary, format availability, license/provenance preview |
| `ScientificFetchRequest` | provider id, source id, desired format, structure/model/metadata selection |
| `ScientificRawAsset` | bytes/URL/resource reference, MIME/format, checksum, source metadata |
| `ScientificParser` | format-specific parsing into an internal scientific model |
| `ScientificModel` | normalized typed model consumed by visualizers and serializers |
| `ScientificVisualizer` | creates/updates render representation without changing scientific facts |
| `ScientificObjectPayloadPatch` | bounded marker/JSON descriptor update, not bulk data storage |

Provider implementation should be split from UI:

```text
Qt UI
  -> registry query
  -> provider
  -> raw asset/cache
  -> parser
  -> scientific model
  -> visualizer/resource flow
  -> bounded marker/JSON descriptor
```

## Lifecycle

| Stage | What happens | Must preserve |
| --- | --- | --- |
| 1. Search | provider searches official index/API | query, provider id, result id, status |
| 2. Metadata | provider fetches summary/details | title/name, source URL, organism/formula/etc., license if available |
| 3. Structure/data fetch | provider downloads selected raw format | original bytes or content-addressed resource, format, version/date |
| 4. Parsing | parser converts raw data to typed model | parser version, warnings, incomplete fields |
| 5. Scientific Model | normalized internal model is created | units, coordinate system, chains/residues/atoms/bonds/properties |
| 6. Cache | raw and parsed artifacts may be cached | checksum, provider, source id, expiry/version policy |
| 7. Visualizer | renderable model/resource/materials are produced | visual settings as derivative, not scientific source |
| 8. Scientific Object | marker/JSON references source and resource | provenance, schema version, bounded descriptor |

Each stage must be able to fail honestly. `Ready` is valid only after the actual selected data path succeeds.

## Storage policy

Current Scientific Object WIP stores metadata in `WorldObject::content`, which is bounded by `WorldObject::MAX_CONTENT_SIZE` and must not become a scientific data warehouse.

### Marker/JSON should store

- `schema_version`;
- `scientific_type`;
- provider id;
- source database name;
- source identifier;
- source URL;
- selected format;
- loaded-at timestamp;
- provider/data version if available;
- license string or URL if available;
- checksum of raw asset or resource;
- content-addressed resource reference for large data;
- parser id/version;
- parse warnings/status;
- visualization settings;
- UI state needed to rehydrate the editor;
- unknown fields from newer schemas.

### Marker/JSON should not store

- API keys;
- OAuth tokens/cookies;
- full PDB/mmCIF/SDF/CIF/MRC/DICOM/NIfTI/LAS payloads;
- huge sequence or point-cloud data;
- generated mesh bytes;
- provider response dumps that exceed the object payload budget;
- user-private local paths unless the user explicitly chose local-only behavior.

### External resources should store

- downloaded structure files (`.sdf`, `.pdb`, `.cif`, `.bcif`, `.mrc`, `.map`, `.ply`, etc.);
- generated visual derivatives (`.bmesh`, generated materials/textures) when needed by the existing resource pipeline;
- large volumes, surfaces, point clouds, medical images and trajectories;
- immutable files addressed by checksum or existing MetaSiberia resource identity.

### Local cache may store

- provider HTTP responses;
- downloaded raw source files;
- parsed intermediate models;
- thumbnails/previews;
- ETag/Last-Modified/version metadata;
- negative-cache entries for unsupported/404 queries.

Local cache must be invalidatable and must not be treated as the source of truth.

### Server cache may store

Only store server-side scientific cache after an explicit architecture decision. If enabled, it must:

- store immutable content by checksum;
- never store user API keys;
- preserve provider terms and license constraints;
- distinguish public cache from private/user-uploaded data;
- expose provenance to clients.

## Provenance fields

Every imported/generated scientific dataset should be able to answer: where did this come from?

Recommended fields:

| Field | Meaning |
| --- | --- |
| `provenance_source` | provider id, e.g. `pubchem`, `rcsb`, `local_file` |
| `provenance_identifier` | CID, PDB ID, UniProt accession, EMDB accession, COD id, material id, etc. |
| `provenance_url` | canonical entry or download/API URL |
| `provenance_author` | depositor/organization/author if provided |
| `provenance_loaded_at` | local load timestamp in ISO 8601 |
| `provenance_format` | raw selected format: SDF, mmCIF, PDB, CIF, FASTA, MRC, JSON, etc. |
| `provenance_version` | provider release/version, model version or entry revision when available |
| `provenance_license` | license or terms reference if available |
| `provenance_checksum` | checksum of raw bytes or durable resource |
| `provenance_parser` | parser id/version used to create internal model |
| `data_origin` | `provider`, `local_file`, `built_in_sample`, `user_generated`, `computed`, `unknown` |

For computed values, also store:

- computation method;
- input source checksums;
- units;
- software/parser version;
- whether result is approximate.

## Internal MetaSiberia model targets

These are recommended internal models, not existing committed classes.

| Internal model | Used for | Typical source formats |
| --- | --- | --- |
| `ScientificMoleculeModel` | small molecules, ligands | SDF, MOL, JSON, XML, SMILES/InChI-derived records |
| `ScientificMacromoleculeModel` | proteins, DNA, RNA, complexes | PDB, mmCIF, BinaryCIF, PDBML/XML |
| `ScientificSequenceModel` | DNA/RNA/protein sequence and annotations | FASTA, GenBank, XML, JSON |
| `ScientificCrystalModel` | crystals, unit cells, symmetry | CIF, POSCAR, provider JSON |
| `ScientificMaterialModel` | calculated material structures/properties | CIF, POSCAR, Materials Project/OQMD JSON |
| `ScientificVolumeMapModel` | density/electron microscopy/volumetric data | MRC, CCP4, VTI, RAW, VDB |
| `ScientificPointCloudModel` | point clouds | LAS/LAZ, PLY, XYZ |
| `ScientificSurfaceModel` | mesh surfaces | STL, OBJ, PLY, glTF |
| `ScientificTableModel` | graphs/tables/plots | CSV, TSV, JSON |
| `ScientificGraphModel` | knowledge/network graphs | GraphML, GEXF, JSON |
| `ScientificGeoModel` | geospatial objects/layers | GeoJSON, Shapefile, raster tile descriptors |
| `ScientificMedicalVolumeModel` | medical imaging | DICOM, NIfTI |

Do not create these as one monolithic class unless the implementation proves that a smaller representation is not enough.

## Provider matrix

Provider rows distinguish external capability from MetaSiberia implementation status. API existence must not be treated as implemented editor support.

| Provider | Types | External capability | MetaSiberia implementation status | Search | 2D | 3D | Properties | Images | Formats | API key |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| PubChem | Molecules | PUG REST/PUG View supports compound search, properties, SDF/JSON/XML/PNG | **Implemented WIP** for Qt Phase 1: interactive CID search/load, property JSON, SDF parse, PNG cache/preview, provenance and object application; network smoke and QWidget apply smoke confirmed `water`/`nicotine`; full manual GUI/server/reconnect still requires owner verification | yes | yes | yes, if conformer exists | yes | PNG | SDF, JSON, PNG | no |
| Built-in sample catalog | Molecules | local MetaSiberia samples only | implemented local demo data; not external scientific provider | local only | no external | local coordinates | small fixed metadata | no | atom/bond tables | no |
| LocalFileProvider | Molecules first, later more | depends on parser | planned/in progress; file picker exists, parsers not claimed here | local path | format-dependent | format-dependent | format-dependent | no | SDF/MOL/XYZ/PDB/etc. planned | no |
| ChEBI | Molecules/ontology | ChEBI 2.0 REST and data products | planned/unsupported in editor | yes externally | limited | not primary 3D source | yes | not primary | JSON/SDF/data products | no public read key documented |
| ChemSpider/RSC | Molecules | RSC developer APIs where account access is available | planned/requires API key | yes externally | depends | depends | yes | depends | JSON/MOL/SDF-like where permitted | yes |
| RCSB PDB | Proteins/DNA/RNA/complexes/ligands | Search/Data/GraphQL/file downloads | planned for later phases; no parser path claimed in editor | yes externally | ligand/entry previews | yes | yes | yes externally | mmCIF/PDB/BCIF/XML/SDF | no public key documented |
| AlphaFold DB | Proteins | predicted structures and confidence metadata | planned Phase 2+ | accession/search | no primary 2D | predicted model | confidence metadata | no primary | mmCIF/PDB/PAE JSON | no |
| UniProt | Sequences/protein metadata | REST sequence/annotation API | planned metadata bridge | yes | no | no structure | yes | no | JSON/FASTA/XML/TSV | no |
| NCBI | DNA/RNA/protein sequence metadata | E-utilities/Datasets/BLAST APIs | planned Phase 3+ | yes | no | no structure | yes | no | FASTA/GenBank/XML/JSON | optional for higher rate |
| COD | Crystals | open CIF structures | planned | yes externally | no | crystal structure | bibliographic/crystal data | no | CIF | no |
| Materials Project | Materials | materials API and `mp-api` | planned/requires credential policy | yes | no | structures | computed properties | no | JSON/CIF/POSCAR-like | yes |
| OQMD | Materials | REST/OPTIMADE data | planned | yes | no | structures where provided | computed properties | no | JSON/OPTIMADE | no credentials for OPTIMADE |
| EMDB | Density/volume maps | metadata APIs and MRC/CCP4 downloads | planned after volume streaming/cache | yes | previews externally | volume maps | metadata | yes externally | MRC/CCP4/XML/mmCIF | no |

### PubChem

| Field | Value |
| --- | --- |
| Назначение | small molecules, compounds, chemical properties, identifiers |
| Official site | <https://pubchem.ncbi.nlm.nih.gov/> |
| API docs | <https://pubchem.ncbi.nlm.nih.gov/docs/pug-rest>, <https://pubchem.ncbi.nlm.nih.gov/docs/programmatic-access> |
| API | PUG REST / PUG View / NCBI PubChem APIs |
| Search | name, CID, SMILES, InChI, formula, structure search where supported |
| Fetch | compound records, properties, SDF, JSON, XML, PNG/2D depiction |
| API key | not required for ordinary PUG REST use; still follow NCBI/PubChem usage guidance |
| Limits/licensing | respect NCBI usage policy, backoff on throttling/errors, avoid bulk loops without batching/cache |
| MetaSiberia provider | `PubChemProvider` |
| Parser | `SDFParser`, `PubChemJsonParser`, optional `PubChemXmlParser` |
| Internal model | `ScientificMoleculeModel` |
| Notes | First online molecule adapter implemented in Qt WIP on 2026-07-11. PubChem uses Windows WinHTTP/SChannel because local QtNetwork SSL is disabled. Phase 1.2 adds exact-first Russian aliases, immediate REST summary/structure, lazy PNG and PUG View sections, section cache/status and normalized modern `SMILES`/`ConnectivitySMILES` response keys. Apply and information-layer smoke passed; full manual GUI/server/reconnect flow still requires owner verification. |

### RCSB Protein Data Bank

| Field | Value |
| --- | --- |
| Назначение | proteins, DNA, RNA, macromolecular complexes, ligands, experimental structures |
| Official site | <https://www.rcsb.org/> |
| API docs | <https://www.rcsb.org/docs/programmatic-access/web-apis-overview>, <https://www.rcsb.org/docs/programmatic-access/file-download-services> |
| API | Search API, Data API, GraphQL Data API, ModelServer, VolumeServer, file downloads |
| Search | PDB ID, text fields, sequence search, structure similarity, organism/taxonomy, chemical components |
| Fetch | JSON metadata, PDBx/mmCIF, BinaryCIF, XML/PDBML, legacy PDB where still available, ligand CIF/SDF |
| API key | not documented as required for public APIs/downloads |
| Limits/licensing | APIs are rate-limited; static files are less restricted; start with few requests/second and handle HTTP 429 |
| MetaSiberia provider | `RcsbProvider` |
| Parser | `MmCifParser`, `PdbParser`, `BinaryCifParser`, `PdbmlParser`, `SDFParser` for ligands |
| Internal model | `ScientificMacromoleculeModel`, `ScientificMoleculeModel` for ligands, `ScientificVolumeMapModel` for volume subsets |
| Notes | Prefer mmCIF/BinaryCIF over legacy PDB for future compatibility. |

### ChEBI

| Field | Value |
| --- | --- |
| Назначение | curated chemical entities, ontology, small molecule metadata/cross-references |
| Official site | <https://www.ebi.ac.uk/chebi/> |
| API docs | <https://www.ebi.ac.uk/chebi/backend/api/docs/> |
| API | ChEBI 2.0 REST API; legacy SOAP web services are retired/deprecated and should not be the primary path |
| Search | keyword, advanced search, ChEBI ID, names, formula, SMILES, InChI/InChIKey, ontology relationships where supported |
| Fetch | JSON entry details via REST; data products include TSV, SDF, ontology files and database dumps |
| API key | not documented as required for public REST read access |
| Limits/licensing | ChEBI data is CC BY 4.0; cache respectfully and preserve citation/license |
| MetaSiberia provider | `ChebiProvider` |
| Parser | `ChebiJsonParser`, `SDFParser`, `Obo/OwlOntologyParser` only if ontology browsing is implemented |
| Internal model | `ScientificMoleculeModel`, optional ontology metadata for `ScientificGraphModel` |
| Notes | Useful as curated vocabulary/ontology companion to PubChem, not necessarily the first 3D structure source. |

### ChemSpider

| Field | Value |
| --- | --- |
| Назначение | small molecules and chemical identifiers from Royal Society of Chemistry/ChemSpider |
| Official site | <https://www.chemspider.com/> |
| API docs | RSC developer portal: <https://developer.rsc.org/>; public docs availability should be rechecked before implementation |
| API | ChemSpider/RSC Compounds APIs where account access is available |
| Search | text/name, registry number, SMILES, InChI, formula and filters supported by ChemSpider/RSC tooling |
| Fetch | compound records, identifiers, MOL/SDF-like structure data where API permits |
| API key | required for API use via RSC developer account |
| Limits/licensing | depends on RSC developer terms/quota; do not implement with a project-wide hardcoded key |
| MetaSiberia provider | `ChemSpiderProvider` |
| Parser | `ChemSpiderJsonParser`, `MolParser`, `SDFParser` |
| Internal model | `ScientificMoleculeModel` |
| Notes | Treat as optional/deferred until API access, quota and terms are confirmed by owner. |

### AlphaFold DB

| Field | Value |
| --- | --- |
| Назначение | predicted protein structures and confidence metadata |
| Official site | <https://alphafold.ebi.ac.uk/> |
| API docs | <https://alphafold.ebi.ac.uk/api-docs>, downloads: <https://alphafold.ebi.ac.uk/download> |
| API | AlphaFold DB API and downloadable files |
| Search | UniProt accession, protein/gene/organism search through website/API where supported |
| Fetch | mmCIF, PDB, PAE JSON, confidence metrics, bulk proteome downloads |
| API key | not documented as required for public access |
| Limits/licensing | preserve AlphaFold/third-party dataset citation and copyright/license metadata; bulk downloads must be explicit |
| MetaSiberia provider | `AlphaFoldProvider` |
| Parser | `MmCifParser`, `PdbParser`, `AlphaFoldPaeJsonParser` |
| Internal model | `ScientificMacromoleculeModel` with prediction-confidence fields |
| Notes | Must label predictions as predicted models, not experimental structures. pLDDT/PAE are visualization/confidence metadata, not measured coordinates. |

### UniProt

| Field | Value |
| --- | --- |
| Назначение | protein sequences, accessions, annotations, cross-references |
| Official site | <https://www.uniprot.org/> |
| API docs | <https://www.uniprot.org/help/api>, <https://www.uniprot.org/help/programmatic_access> |
| API | UniProt REST API |
| Search | accession, protein name, gene, organism, taxonomy, reviewed/unreviewed filters |
| Fetch | JSON, FASTA, XML, RDF, TSV/GFF-style outputs where supported |
| API key | not required for public read access |
| Limits/licensing | follow UniProt usage/format guidance; preserve entry accession and release/version when available |
| MetaSiberia provider | `UniProtProvider` |
| Parser | `UniProtJsonParser`, `FastaParser`, optional `UniProtXmlParser` |
| Internal model | `ScientificSequenceModel`, protein annotation metadata for macromolecule search |
| Notes | Best used as sequence/metadata provider and bridge to AlphaFold/RCSB, not as a structure provider by itself. |

### NCBI Entrez / NCBI Datasets

| Field | Value |
| --- | --- |
| Назначение | nucleotide/protein sequences, genes, taxonomy, literature-linked biological metadata |
| Official site | <https://www.ncbi.nlm.nih.gov/> |
| API docs | <https://www.ncbi.nlm.nih.gov/home/develop/api/>, E-utilities: <https://www.ncbi.nlm.nih.gov/books/NBK25497/>, Datasets API: <https://www.ncbi.nlm.nih.gov/datasets/docs/v2/api/> |
| API | Entrez E-utilities, BLAST URL API, NCBI Datasets API |
| Search | Entrez search by accession, gene, organism, taxonomy, sequence/literature fields |
| Fetch | FASTA, GenBank, XML/JSON where supported, sequence records and metadata |
| API key | optional for E-utilities; required only for higher default rate |
| Limits/licensing | E-utilities: 3 requests/sec without API key; 10 requests/sec with key by default; include registered tool/email where required |
| MetaSiberia provider | `NcbiProvider` |
| Parser | `FastaParser`, `GenBankParser`, `NcbiXmlParser`, `NcbiJsonParser` |
| Internal model | `ScientificSequenceModel`, optional metadata tables |
| Notes | Must use batching/history for large record sets; never fire one request per UI repaint. |

### Crystallography Open Database / COD

| Field | Value |
| --- | --- |
| Назначение | open crystal structures for organic, inorganic, metal-organic compounds and minerals, excluding biopolymers |
| Official site | <https://www.crystallography.net/cod/> |
| API/docs | COD site/wiki links: querying, obtaining COD, mirrors, CC0 license |
| API | COD search/query endpoints and downloadable CIF files; exact endpoint must be verified during implementation |
| Search | COD ID, formula, chemical name, bibliographic/crystallographic fields where supported |
| Fetch | CIF files, database/mirror downloads where appropriate |
| API key | not required for public access |
| Limits/licensing | COD states data are public domain/CC0; still preserve COD ID and source URL |
| MetaSiberia provider | `CodProvider` |
| Parser | `CifParser` |
| Internal model | `ScientificCrystalModel`, sometimes `ScientificMoleculeModel` preview |
| Notes | First candidate for open crystal import after CIF parser exists. |

### Materials Project

| Field | Value |
| --- | --- |
| Назначение | computed materials structures and properties |
| Official site | <https://next-gen.materialsproject.org/> |
| API docs | <https://docs.materialsproject.org/downloading-data/using-the-api/getting-started>, Swagger: <https://api.materialsproject.org/docs> |
| API | Materials Project API, `mp-api`/`MPRester` client |
| Search | material id, formula/composition, elements, property availability, summary endpoints |
| Fetch | JSON documents, structures/properties, CIF/POSCAR-like data through client/tooling where available |
| API key | required per Materials Project account |
| Limits/licensing | API rate limits start around 25 requests/sec; use field restriction, list queries and local copies for large jobs |
| MetaSiberia provider | `MaterialsProjectProvider` |
| Parser | `MaterialsProjectJsonParser`, `CifParser`, `PoscarParser` |
| Internal model | `ScientificMaterialModel`, `ScientificCrystalModel` |
| Notes | Requires owner/user credential design before implementation. API keys must stay outside marker/JSON. |

### OQMD

| Field | Value |
| --- | --- |
| Назначение | computed thermodynamic and structural properties of materials |
| Official site | <https://oqmd.org/> |
| API docs | REST: <https://static.oqmd.org/static/docs/restful.html>, OPTIMADE: <https://oqmd.org/optimade/> |
| API | OQMD RESTful API and OPTIMADE endpoints |
| Search | formation energy endpoint filters, element sets, composition/material queries, OPTIMADE structure filters |
| Fetch | JSON result pages/records, structure/material properties where endpoint supports them |
| API key | OPTIMADE page states no user credentials are required |
| Limits/licensing | OQMD data is CC BY 4.0; handle pagination and cache repeated queries |
| MetaSiberia provider | `OqmdProvider` |
| Parser | `OqmdJsonParser`, optional `OptimadeJsonParser` |
| Internal model | `ScientificMaterialModel`, `ScientificCrystalModel` |
| Notes | Good candidate for keyless materials metadata after core material model exists. |

### EMDB

| Field | Value |
| --- | --- |
| Назначение | 3D electron microscopy maps/volumes and metadata |
| Official site | <https://www.ebi.ac.uk/emdb/> |
| API docs | <https://www.ebi.ac.uk/emdb/api/>, docs: <https://www.ebi.ac.uk/emdb/documentation> |
| API | EMDB REST APIs for archive/quality/annotation metadata; file downloads for maps |
| Search | EMDB accession, title, sample, organism, method/resolution metadata through EMDB search/API |
| Fetch | metadata XML/PDBx/mmCIF, MRC/CCP4 `.map` volumes, validation/annotation metadata where available |
| API key | not documented as required for public read access |
| Limits/licensing | large files; use explicit download confirmation/cache and preserve wwPDB/EMDB citations |
| MetaSiberia provider | `EmdbProvider` |
| Parser | `EmdbJsonParser`, `EmdbXmlParser`, `MrcCcp4Parser`, optional `MmCifParser` for metadata |
| Internal model | `ScientificVolumeMapModel`, optional linked `ScientificMacromoleculeModel` refs |
| Notes | Never load full maps into marker/JSON; always external resource/cache. |

### Local file provider

| Field | Value |
| --- | --- |
| Назначение | user-provided files without network dependency |
| Official site | none |
| API docs | format-specific docs/parsers |
| API | local file picker/import path |
| Search | local path selection, optional directory scan if explicitly requested |
| Fetch | local file bytes |
| API key | not applicable |
| Limits/licensing | local files may be private/proprietary; provenance should say `local_file` and avoid leaking full path if object will be shared |
| MetaSiberia provider | `LocalFileProvider` |
| Parser | dispatch by format/magic/header, not extension only |
| Internal model | all supported internal models |
| Notes | Best first implementation path for formats because it is reproducible offline and testable with fixtures. |

Recommended initial format priorities:

| Object type | First import formats |
| --- | --- |
| Molecule | SDF, MOL, XYZ |
| Protein/DNA/RNA | mmCIF, PDB, FASTA |
| Crystal/material | CIF, POSCAR |
| Volume/density | MRC/CCP4 |
| Surface | OBJ, STL, PLY |
| Point cloud | PLY, XYZ, LAS/LAZ if dependency approved |
| Table/plot | CSV, TSV, JSON |
| Graph | GraphML, GEXF, JSON |
| GIS | GeoJSON first; Shapefile only with dependency plan |
| Medical | NIfTI before DICOM directory workflows unless medical requirements are clarified |

## Provider status vocabulary

Use these labels consistently in UI/docs:

| Status | Meaning |
| --- | --- |
| `implemented` | provider path, parser, provenance and error handling exist and were verified |
| `partial` | some path exists but scope/format/error handling is incomplete |
| `placeholder` | UI vocabulary exists, no real provider call/import |
| `unsupported` | known unsupported combination; must not mutate object data |
| `requires_api_key` | implementation depends on user/owner credential |
| `requires_external_library` | parser/provider depends on unapproved dependency |
| `requires_runtime_verification` | builds/static checks passed, live UI/network behavior not verified |

`placeholder` must never be presented as `implemented`.

## Credentials and API keys

Rules:

- API keys stay in local user settings, OS credential store, environment variables, or owner-approved secret storage.
- API keys are never serialized into `WorldObject::content`.
- API keys are never logged, copied to docs, or stored in fixtures.
- Shared/server-side provider credentials require ADR/security review.
- Provider UI should show whether a key is required before search.

Known key requirements:

| Provider | Key |
| --- | --- |
| PubChem | no ordinary PUG REST key |
| RCSB PDB | no public key documented |
| ChEBI | no public read key documented |
| AlphaFold DB | no public key documented |
| UniProt | no public key documented |
| NCBI E-utilities | optional key for higher rate |
| COD | no public key documented |
| Materials Project | required account API key |
| OQMD | no credentials for OPTIMADE per official page |
| EMDB | no public key documented |
| ChemSpider/RSC | API key required for API use |

## Rate limiting and network policy

Minimum rules for all providers:

- central throttling per provider id;
- timeout and cancellation;
- exponential backoff on 429/503/network errors;
- no network request from render/frame loop;
- no request per text-field keystroke without debounce;
- no unbounded auto-pagination;
- cache repeated search/fetch results;
- show provider/source status in UI.

Known explicit limits or guidance:

| Provider | Policy known on 2026-07-10 |
| --- | --- |
| NCBI E-utilities | 3 requests/sec without API key; 10 requests/sec with key by default |
| RCSB PDB | API rate limits exist; start with a handful of requests/sec; handle HTTP 429 |
| Materials Project | rate limits start at 25 requests/sec; prefer list queries, limited fields and local copies |
| OQMD | use pagination and avoid full-database loops through UI |
| EMDB | large map downloads require explicit user action/cache |

For providers without a published number in this document, implement conservative throttling and document the exact source when discovered.

## Parser policy

Parsers must:

- distinguish syntax error from unsupported feature;
- preserve original identifiers;
- keep units and coordinate systems explicit;
- store warnings for incomplete data;
- avoid fabricating bonds or properties unless the method is explicitly marked computed;
- identify whether coordinates are 2D, 3D, predicted, experimental or user-generated;
- retain raw asset checksum;
- be testable offline using small fixtures.

If a parser infers bonds, secondary structure, charge, surface or missing hydrogens, the result must be marked as computed/inferred and include method metadata.

## Visualizer policy

Visualization is derivative.

The visualizer may:

- generate meshes/materials;
- apply CPK/chain/residue/property colours;
- create labels/legend;
- downsample large data for display;
- create LOD representations;
- cache generated resources.

The visualizer must not:

- overwrite source scientific data;
- replace failed data with demo geometry;
- change coordinates unless transformation is explicit;
- hide low confidence/incomplete data as if it were authoritative;
- make AlphaFold predicted structures look like experimental PDB structures without visible provenance.

## Recommended implementation order

Do not implement every provider at once.

Recommended low-risk sequence:

1. `LocalFileProvider` with SDF/MOL/XYZ and mmCIF/PDB fixtures.
2. `PubChemProvider` for small molecules via PUG REST.
3. `RcsbProvider` for metadata + mmCIF/PDB download.
4. `ChebiProvider` for curated molecule metadata/cross-reference.
5. `AlphaFoldProvider` for predicted protein structures and confidence metadata.
6. `NcbiProvider`/`UniProtProvider` for sequence search/metadata.
7. `CodProvider` with CIF parser.
8. `OqmdProvider` and `MaterialsProjectProvider` after material model and credential policy.
9. `EmdbProvider` after volume resource streaming/cache exists.
10. ChemSpider/RSC only after owner confirms API access and terms.

Each step should include:

- provider descriptor;
- search and fetch path;
- at least one parser;
- provenance fields;
- cache policy;
- offline fixtures;
- unsupported/error UI states;
- documentation update.

## Test and verification expectations

For each provider, create tests/fixtures before claiming implementation:

- valid known ID;
- unknown ID;
- provider error/offline response;
- malformed payload;
- rate-limit/throttle handling where mockable;
- parser warning for incomplete fields;
- provenance checksum;
- no API key in object content/log;
- no demo fallback mutation;
- round-trip marker/JSON preservation.

Runtime verification should include:

- search UI;
- result selection;
- object update;
- reload/reconnect;
- cache hit;
- failed request;
- unsupported type/provider pair;
- visual changes without scientific data mutation.

## Documentation update rules

When a provider changes:

- update this document if API/source/provider policy changes;
- update [scientific-object-editor.md](scientific-object-editor.md) if implementation status changes;
- update [data-map.md](data-map.md) if persistence/resource/cache schema changes;
- update [component-relations.md](component-relations.md) if dependencies change;
- update [engineering-debt.md](engineering-debt.md) if provider limitations remain;
- create ADR only for large architectural decisions: server-side cache, new shared scientific schema, new dependency family, credential architecture, protocol/storage change.

## Official sources reviewed

- PubChem PUG REST / programmatic access: <https://pubchem.ncbi.nlm.nih.gov/docs/pug-rest>, <https://pubchem.ncbi.nlm.nih.gov/docs/programmatic-access>
- NCBI APIs and E-utilities limits: <https://www.ncbi.nlm.nih.gov/home/develop/api/>, <https://www.ncbi.nlm.nih.gov/books/NBK25497/>
- RCSB PDB APIs and downloads: <https://www.rcsb.org/docs/programmatic-access/web-apis-overview>, <https://www.rcsb.org/docs/programmatic-access/file-download-services>
- ChEBI 2.0 API and licensing: <https://www.ebi.ac.uk/chebi/backend/api/docs/>, <https://www.ebi.ac.uk/about/news/updates-from-data-resources/chebi-2-0-launches/>
- AlphaFold DB API/downloads: <https://alphafold.ebi.ac.uk/api-docs>, <https://alphafold.ebi.ac.uk/download>
- UniProt API/help: <https://www.uniprot.org/help/api>, <https://www.uniprot.org/help/programmatic_access>
- COD: <https://www.crystallography.net/cod/>
- Materials Project API: <https://docs.materialsproject.org/downloading-data/using-the-api/getting-started>, <https://docs.materialsproject.org/downloading-data/using-the-api/tips-for-large-downloads>, <https://api.materialsproject.org/docs>
- OQMD REST/OPTIMADE: <https://static.oqmd.org/static/docs/restful.html>, <https://oqmd.org/optimade/>
- EMDB API/data model: <https://www.ebi.ac.uk/emdb/api/>, <https://www.ebi.ac.uk/emdb/documentation/data_models>
- ChemSpider/RSC entry points: <https://www.chemspider.com/>, <https://developer.rsc.org/>
