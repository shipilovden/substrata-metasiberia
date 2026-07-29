# Gaussian Splatting in Metasiberia

Статус: **implemented on `feature/gaussian-splat-native-web`; local Qt5, web/Emscripten and Windows server builds verified on 2026-07-29.**

## What Works

Metasiberia now treats Gaussian splats as first-class world objects, not as a browser-only prototype.

- Native Qt5 client loads and renders Gaussian splats in the normal 3D scene.
- Web/Emscripten client has an ImGui "Gaussian splats" upload panel.
- Uploaded splats become ordinary editable world objects: select them, then use the existing transform controls for position, rotation and scale.
- Native Qt5 selection opens a dedicated **Редактор GaussianSplats** panel with safe Original/Clean Haze/Metasiberia Clear presets, reset, SH detail, source-opacity filtering, density, brightness, splat radius, saturation, contrast and edge-alpha controls.
- Server/shared resource validation preserves Gaussian splat resources and SOG metadata entry points.
- Existing OBJ/GLTF/GLB/VOX/STL/IGMESH/image loading remains on the old path.

## Supported Formats

The supported format boundary follows the self-hosted SplatTransform converter bundle:

- direct native decode: standard 3DGS binary `.ply`, `.splat`, SPZ v4 `.spz`;
- converter-backed decode: compressed PLY, `.ksplat`, SPZ v2-v3, bundled `.sog`, unbundled SOG `meta.json`, streamed SOG `lod-meta.json`, `.lcc`, `.lcc2`.

Do not classify arbitrary JSON files as Gaussian resources. Only exact basenames `meta.json` and `lod-meta.json` are accepted as SOG entry points.

## Native Qt5 Path

Primary files:

- `shared/GaussianSplatAsset.*`: format detection and PLY header validation.
- `shared/GaussianSplatData.*`: bounded direct decode for PLY/SPLAT/SPZ v4, render settings and SH data.
- `gui_client/GaussianSplatCEFConverter.*`: hidden 1x1 off-screen CEF converter for all other supported containers.
- `gui_client/GaussianSplatRenderer.*`: OpenGL instanced splat rendering.
- `gui_client/AddObjectDialog.*`: Add Object dialog integration and progress UI.
- `gui_client/ObjectEditor.*` and `gui_client/MainWindow.cpp`: standard transform editor plus the dedicated **Редактор GaussianSplats** panel.

Gaussian colour data in `GaussianSplatData` remains in unclamped display space. PLY/SPZ DC and `f_rest_*` terms are combined first; only the final view-dependent colour is clamped and converted to linear RGB in the shader. Clamping or linearising DC before SH evaluation destroys directional colour detail.

Gaussian render tuning is stored in `WorldObject::content` as:

```text
gaussian_splat_settings_v1
opacity_multiplier=1
brightness=1
radius_multiplier=1
saturation=1
contrast=1
alpha_cutoff=0.00392157
minimum_source_opacity=0
sh_degree_override=-1
```

This keeps Gaussian splats compatible with the existing server protocol: the object remains a normal generic world object, but the native client recognises the Gaussian model URL and applies the specialised editor/render settings.

Rendering notes:

- Gaussian objects must never use the engine's weighted OIT pass. OIT averages hidden front/back colours and creates the milky, X-ray appearance that cannot be repaired by increasing density.
- The native renderer sorts splat centres far-to-near for the current camera direction and uses the ordinary source-over alpha queue (`transparent=false`, `alpha_blend=true`, depth test enabled, depth writes disabled by that queue).
- The per-instance order is refreshed when the camera direction changes by about two degrees. The instance matrix attribute carries the sorted source index; its VAO therefore requires the standard instancing attributes 5–8.
- Density preserves the source at `1` and uses `1-pow(1-opacity,density)`.
- `minimum_source_opacity` removes whole weak source splats before density is applied. This is the control for haze/floaters.
- `alpha_cutoff` only clips the low-alpha Gaussian tail around each rendered splat; it does not remove weak source splats.
- `sh_degree_override=-1` preserves the source SH degree. Values `0..3` cap view-dependent colour detail without changing the source asset.

For a newly uploaded asset, start at **Original / SuperSplat default**. Older objects may still contain experimental high-density/contrast settings in `WorldObject::content`; select them and press **Reset to Original** once after upgrading.

The CEF converter is not a visible browser and does not create a WebView object. It loads the packaged converter from:

```text
data/resources/gaussian_splat/gaussian_splat_converter.js
data/resources/gaussian_splat/webp.wasm
```

On Windows, derive the converter's input directory from the already canonical
input-file path. Do not call `FileUtils::getCanonicalPath()` on that directory:
its regular-file handle path produces `ERROR_ACCESS_DENIED (5)` without
`FILE_FLAG_BACKUP_SEMANTICS`, even when the directory ACL is valid.

## Web Client Path

Primary files:

- `webserver_public_files/gaussian_splat_converter.js`: self-hosted converter bundle.
- `gui_client/SDLClient.cpp`: Emscripten file/folder picker, progress status and C++ bridge.
- `gui_client/CMakeLists.txt`: Emscripten exports for `processGaussianSplatFile` and `setGaussianSplatWebStatus`.

In the web client, choose:

- "Add Gaussian splat file..." for single-file `.ply`, `.splat`, `.ksplat`, `.spz`, `.sog`, `.lcc`, `.lcc2`;
- "Add sidecar folder..." for SOG/LCC packages that depend on sidecars, especially `meta.json`, `lod-meta.json`, nested `.webp` files or chunk folders.

The browser converter normalises the source to canonical binary PLY bytes, then calls the same `GUIClient::createModelObject()` flow as native loading.

## Verified Builds

Qt5 canonical wrapper:

```powershell
powershell -ExecutionPolicy Bypass -File C:\programming\qt_build.ps1 `
  -RepoRoot C:\programming\substrata_gaussian_master `
  -BuildDir C:\programming\substrata_build_qt_gaussian `
  -OutputRoot C:\programming\substrata_output_qt `
  -SkipLegacyOutputMirror
```

Expected owner launch path:

```text
C:\programming\substrata_output_qt\vs2022\cyberspace_x64\RelWithDebInfo\gui_client.exe
```

Verified on 2026-07-29:

- `C:\programming\substrata_output_qt\build_manifest.json`: `success=true`, `runtime_ready=true`;
- branch `feature/gaussian-splat-native-web`;
- `gui_client.exe` SHA-256: `5A2B4DCE0626B6ABE35F2357D9CD993302B7F537B6A01D336FA016AE088B4014`;
- local Windows `server` target SHA-256: `07E7EC80CEF19F39D726B9798F3858E2257440E7E6761F05CA63ACB830C53628`;
- Emscripten output: `C:\programming\substrata_output_emscripten_gaussian\test_builds\gui_client.js/.wasm/.data`.
- `GaussianSplatDecoder::test()` passed in the local server test run. A later, unrelated legacy Basis test still expects a hard-coded `D:\files\...` output path and stops the complete suite.
- The packaged Gaussian vertex/fragment shaders compiled and linked successfully through the local NVIDIA GeForce GTX 1650 OpenGL 4.3 driver.

## Production Rule

Production Metasiberia runs a Linux ELF `server` on `metasiberia-server`, not Windows `server.exe`. This branch has only been built locally. Do not deploy or restart production services without an explicit current confirmation from the owner.

For production rollout, first build Linux `server` using the documented `/srv/metasiberia/build/master` workflow, stage a release directory, then restart and verify `7600/8080/8443`.
