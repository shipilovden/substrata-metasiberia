# Map World OSM Ground Layer (2026-04-22)

Scope: special world `sub://vr.metasiberia.com/map`

## Goal
- Render a street map directly on the ground surface only in `sub://vr.metasiberia.com/map`.
- Keep the rest of Metasiberia worlds unchanged.
- Make the map appear quickly from coarse coverage first, then refine nearby detail without white holes.

## Current Client Architecture
- `gui_client/MapWorldLayer.*` owns a stack of world-space quads rendered slightly above the terrain only in `sub://vr.metasiberia.com/map`.
- The map centre is hardcoded to `53.691717, 87.432949`.
- Local world coordinates are treated as metres in Web Mercator space around that centre.
- Tile textures are fetched directly from `https://tile.openstreetmap.org/{z}/{x}/{y}.png`.
- Layer zooms are currently stacked as coarse-to-fine OSM levels so the horizon can fill first and the area around the avatar can refine afterwards.

## 2026-04-23 Update
- Reverted the experimental switch to server `/tile` screenshots. That path matches the parcel website map, not the street-map ground layer requested for this world.
- Restored direct OSM tile fetching inside `MapWorldLayer`.
- Kept retry scheduling for tiles that are still missing locally or previously failed to download.
- Changed tile assignment so a tile can keep using the nearest loaded parent tile instead of dropping to a blank cell while a finer tile is still loading.
- Reordered the active zoom stack so coarse coverage is requested first.
- `Go -> Map World` still spawns at local `x=0, y=0, z=1.67`, which corresponds to the hardcoded centre above.

## Important Behavioural Note
- The hard part is not placing a texture on the ground. The difficult part is seamless tile streaming while the avatar moves:
  - coarse tiles must cover the horizon first,
  - finer tiles must refine without hiding already-loaded coarse coverage,
  - failed or not-yet-cached HTTP tiles must be retried,
  - and all of this must happen only in the map world without affecting the rest of the client.

## Known Limits
- The map centre is still hardcoded in the client.
- OSM network latency and third-party tile availability still affect first-time loads.
- A dedicated UI for choosing map centre, zoom policy, and cache behaviour is still pending.
