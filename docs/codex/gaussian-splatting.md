# Gaussian Splatting: native integration plan

Статус: **Phase 0/1 — shared resource boundary only**.

В upstream `glaretechnologies/substrata` готового 3DGS renderer нет. Поэтому
интеграция разбивается на изолированные этапы:

1. shared/server принимают явно распознаваемые `.ply`, compressed `.ply`, `.splat`, `.ksplat`, `.spz`, `.sog`, `.lcc` и `.lcc2`;
2. client получает bounded PLY header validator и metadata descriptor;
3. native OpenGL renderer добавляется за feature flag, с fallback на обычные
   mesh-объекты и без изменения текущего render path;
4. после локального GPU smoke-check добавляются dynamic upload и transform
   editor; затем отдельный production rollout.

На текущем этапе новые расширения добавлены только в общую проверку типов
ресурсов. Existing mesh loading не перенаправляется на 3DGS, пока renderer не
готов — это намеренно сохраняет рабочий клиент.
