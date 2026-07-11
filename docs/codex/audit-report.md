# Отчёт аудита документации и архитектуры

Назначение: evidence snapshot, на котором основана Documentation Migration Phase 2/3.

Дата: 2026-07-10

Baseline: `master` / `2ef62fd6`, `origin/master` на том же commit

Режим: read-only source analysis + docs/AGENTS edits; без build/test/client/server/deploy/production

## Scope

- top-level Git/CMake owners, executable/library composition;
- Native/Web Client boundaries и editor surfaces;
- Realtime + embedded HTTP/WS/Server Website/Admin process boundary;
- shared protocol v62, `WorldObject` and persistence contracts;
- assets/build/web/release/operations stages;
- current dirty Particle и Scientific Object integration;
- all `docs/codex`, MCOS и repository/local `AGENTS.md`;
- first-party documentation roles, stale paths и relative links.

Binary/media/vendor contents и secret values не исследовались. Production не опрашивался.

## Сохранность рабочего дерева

До Phase 2 уже существовали modified client/particle files, modified first-party docs, четыре untracked Scientific Object files и untracked Phase 1 docs/AGENTS. Они принадлежат пользователю. Code changes не удалялись, не откатывались и не форматировались; документационная миграция не меняет runtime code.

## Подтверждённая архитектура

- Native и Web Client — surfaces одной `gui_client` codebase/target.
- Server Website/Admin/HTTP/WS и realtime server — логические areas одного `server` process.
- Persistence — custom binary record database, не SQL.
- Public Website `metasiberia.com` и TheRift source отсутствуют в repo.
- `shared` — critical source contract, не standalone library/deployable.
- website assets disk-loaded и deploy отдельно от binary.
- Scientific Object WIP — generic WorldObject marker/JSON, текущая интерпретация в Qt client и existing generic server persistence.

## Scientific Object: новые подтверждённые знания

- Marker: `metasiberia_scientific_object_v1`.
- Add action, editor lifecycle, MOC/CMake registration и generic update flow присутствуют в dirty tree.
- Settings реально содержат metadata/source/tables/visualisation/measurement/animation/simulation/AI/custom fields.
- Online/database и AI behavior — local mocks/templates; provider/network execution отсутствуют.
- Molecule atom/bond tables могут генерировать temporary OBJ/MTL и preview materials; общий `objectEdited()` path конвертирует model в checksum-addressed `.bmesh`.
- `WorldObject::MAX_CONTENT_SIZE` = 10 000 bytes конфликтует с суммарной ёмкостью text fields.
- AI key хранится local QSettings per provider и не входит в object content; security review отсутствует.
- Server/shared не имеют special Scientific Object parser/type/message.

## Устранённые противоречия

- MCOS source priority дополнен real architecture и confirmed data.
- MCOS Web Client исправлен с «самостоятельной реализации» на shared codebase.
- Логические HTTP/WS/Admin roles отделены от выдуманного standalone deployment/API.
- Public Website отделён от Server Website assets.
- Scientific Object цели MCOS помечены normative WIP, а не current capabilities.
- Удалён дублирующий MCOS task-template appendix и двойной финальный комментарий.
- Scientific Object больше не описывается как «случайные untracked files»; статус официальный WIP с точной границей.
- Project maps больше не смешивают каталог, target, process и external service.

## Открытые architecture/data gaps

1. Scientific content size enforcement/schema migration/unknown fields.
2. Runtime/reconnect validation content-addressed flow generated molecule models.
3. Real scientific adapters/import/execution/security boundaries.
4. `generic_page_config.xml` references absent `features.htmlfrag`.
5. `server_dist_resources` -> runtime staging owner не найден.
6. CI configure-only; compile/test/package/runtime отсутствуют.
7. Root-disabled tools/installer buildability и ownership не подтверждены.
8. Public Website/TheRift/avatar service external source boundaries неизвестны.
9. Reproducible current Web Client production deploy не подтверждён.
10. Auth/session/local credential security требует отдельного review.

## Устаревшие области

- v2 deploy paths/scripts и `/root/cyberspace_server_state` как current production flow;
- direct OSM client URLs, host-specific Basis workaround, old XR camera/avatar advice;
- Qt6 complete/dual-support claims;
- tracked Emscripten caches/local backups as source of truth;
- plans/UI labels as evidence implemented adapters/services.

Historical документы сохранены с current warnings; release notes не переписывались.

## Чувствительные зоны

- runtime state/config/credentials/resources/media;
- local QSettings, chat history, traces/logs/recordings;
- auth/VRoid/external integration code;
- map maintenance sessions, authenticated site capture, local backups/fuzz seeds;
- Scientific AI QSettings key storage.

Документация не содержит secret values. Path-only absence стандартных token files не является security proof.

## Ограничения аудита

- Build/tests запрещены задачей, поэтому compile/runtime readiness не доказана.
- Production/live URLs/services не проверялись.
- Scientific files untracked и могут измениться до commit.
- Current dirty Particle changes анализировались только для границы с Scientific/docs, не как отдельный feature audit.
- External dependencies/vendored implementation не исследовались.

## Manual follow-up владельца

1. После завершения WIP — review [scientific-object-editor.md](scientific-object-editor.md) и подтвердить intended scope.
2. Разрешённые позже Qt build/manual flow checks для Scientific/Particle changes.
3. Scientific payload 10 KB, malformed JSON, reconnect/resource portability и credential leakage tests.
4. Решить absent `features.htmlfrag` и server seed staging.
5. Подтвердить external Public Website/TheRift source ownership.
6. Отдельно подтвердить live production state/services только перед production task.
7. Security review auth/session/QSettings credentials.
8. Проверить font/media/model licenses/provenance.

## Самопроверка аудита

- Fact/partial/WIP/plan/history/unknown разделены.
- Code имеет приоритет над MCOS/docs.
- Scientific UI vocabulary не принят за implementation.
- Public/Server Website/Admin и logical/physical dependencies разделены.
- Новые устойчивые знания перенесены в canonical docs.
- Source functionality, build outputs и production не изменялись.

Пофайловый итог Phase 2/3: [verification-report.md](verification-report.md).
