# План: CAD, инженерные чертежи и промышленные объекты

Статус: архитектурный план, массовую реализацию начинать только после утверждения зависимостей и security boundary.

## Цель и границы

Добавить Industrial Object и CAD Editor в существующую левую панель MetaSiberia. Исходный CAD, точная BREP-геометрия, визуальный mesh, чертежи, BOM и документы — разные представления одного объекта. GLB/mesh является производным preview и не заменяет исходный CAD.

## Модель

`IndustrialObject`, `CADDocument`, `CADPart`, `CADAssembly`, `CADAssemblyNode`, `CADGeometry`, `CADTopology`, `CADMaterial`, `CADEngineeringProperty`, `CADParameter`, `CADConstraint`, `CADDimension`, `CADTolerance`, `CADAnnotation`, `CADDrawing`, `CADSheet`, `CADView`, `CADLayer`, `CADBOM`, `CADBOMItem`, `CADSourceFile`, `CADConversionResult`, `CADMeasurement`, `CADIssue`, `CADRevision`.

IndustrialObject хранит UUID, тип, производителя, part/product number, version/revision, units/coordinate system, source and converted resources, visual mesh, BREP reference, assembly tree, materials, mass/volume/center of mass/bounds, documents, drawings, BOM, license, provenance and import timestamps.

## Поддержка форматов

| Формат | План |
|---|---|
| STEP AP203/AP214/AP242 | первый точный CAD-импорт через Open CASCADE |
| IGES | следующий BREP-импорт через Open CASCADE |
| BREP | внутренний точный обмен |
| DXF | отдельный 2D drawing importer |
| SVG | векторное отображение/экспорт чертежей |
| PDF | только отображение/документ, не CAD-геометрия |
| DWG | только после отдельного лицензированного SDK |
| GLB/glTF/OBJ/STL | производные или mesh-only assets; не считать полноценным CAD |
| FreeCAD TechDraw | исследовательское направление для чертежей |

Нельзя заявлять поддержку формата без реально подключённого parser/SDK и fixture-тестов.

## Поток импорта

```text
file picker -> size/MIME/extension/security checks -> isolated temp storage
  -> background import queue -> units/assembly/materials/metadata extraction
  -> BREP validation -> visual mesh + LOD -> report/warnings
  -> immutable source resource + cached conversion -> IndustrialObject descriptor
```

Импорт не блокирует Qt UI. Нужны progress, cancellation, retry, logs, warnings, revision comparison и сохранение исходного файла.

## CAD-редактор

Вкладки: Основное, Структура, Геометрия, Материалы, Параметры, Чертежи, Спецификация, Измерения, Аннотации, Документы, Ревизии, Экспозиция.

Основные действия: добавить промышленный объект, импортировать CAD/чертёж, обновить source, rebuild mesh, показать структуру, скрыть/изолировать деталь, измерить, создать сечение, добавить annotation, открыть drawing/BOM, сравнить revisions, экспортировать GLB-preview, проверить модель и открыть raw metadata.

## Измерения

По BREP: точные расстояния, площади, объёмы, радиусы, диаметры, толщины, зазоры, пересечения и mass properties при наличии материала. По mesh: только приближённые bounds, surface/volume и screen-space измерения; UI обязан показывать `approximate`.

## Производительность

Для больших сборок: background conversion, content-addressed cache, LOD, instancing, mesh merging, spatial hierarchy, frustum/occlusion culling, lazy/progressive loading, part streaming и optional Draco/meshoptimizer. Исходный CAD и visual GLB хранятся раздельно.

## Безопасность

Ограничить размер и память, проверять MIME/magic bytes, запрещать embedded code/macros, использовать изолированный converter process, timeout, temporary directory cleanup, archive/zip-bomb checks, audit log и проверку зависимостей. Не запускать конвертацию в UI-потоке и не принимать клиентские результаты как authoritative без серверной проверки.

## Связь с образованием

`QuestObjective` может ссылаться на IndustrialObject, CADPart, AssemblyNode, Measurement, Annotation, Drawing или BOM item: найти деталь, измерить отверстие, определить материал, сравнить ревизии, прочитать чертёж, заполнить BOM, выполнить виртуальное обслуживание.

## Зависимости и файлы

Первый этап: Open CASCADE, importer/parser layer, background job queue, resource/cache adapter и `IndustrialObjectEditor.*`. Интеграционные точки клиента — `MainWindow`, existing object editor dock, `WorldObject`, `ResourceManager`, `GUIClient::objectEdited()` и обычная object creation/update. Серверные readers/writers, resource permissions и revision metadata проектируются отдельным ADR; при изменении `server/**`, `shared/**` или protocol обязательна Linux-сборка server по регламенту.

## Этапы

0. Аудит форматов, лицензий, security sandbox и storage.
1. Local STEP fixture → BREP validation → GLB preview → generic world object.
2. Assembly tree, parts, materials, LOD and lazy loading.
3. DXF/SVG/PDF drawing viewer, sheets, dimensions and annotations.
4. BOM, documents, revisions, issues and comparison.
5. Exact measurements, sections and educational quest links.
6. Large-assembly streaming, server preprocessing and production hardening.

## Минимальный CAD MVP

Импорт одного STEP-файла в фоне, сохранение исходника, извлечение assembly tree, построение GLB-preview, размещение IndustrialObject в мире, просмотр структуры и простое измерение с явной пометкой точности.
