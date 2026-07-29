# Gaussian Splatting in Metasiberia

Статус: **implemented on `feature/gaussian-splat-native-web`; local Qt5, web/Emscripten and Windows server builds verified on 2026-07-29.**

## What Works

Metasiberia now treats Gaussian splats as first-class world objects, not as a browser-only prototype.

- Native Qt5 client loads and renders Gaussian splats in the normal 3D scene.
- Web/Emscripten client has an ImGui "Gaussian splats" upload panel.
- Uploaded splats become ordinary editable world objects: select them, then use the existing transform controls for position, rotation and scale.
- Native Qt5 selection opens a dedicated **Редактор GaussianSplats** panel with Gaussian-specific density/opacity, brightness and splat radius controls.
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
- `gui_client/GaussianSplatDecoder.*`: direct decode for PLY/SPLAT/SPZ v4.
- `gui_client/GaussianSplatCEFConverter.*`: hidden 1x1 off-screen CEF converter for all other supported containers.
- `gui_client/GaussianSplatRenderer.*`: OpenGL instanced splat rendering.
- `gui_client/AddObjectDialog.*`: Add Object dialog integration and progress UI.
- `gui_client/ObjectEditor.*` and `gui_client/MainWindow.cpp`: standard transform editor plus the dedicated **Редактор GaussianSplats** panel.

Gaussian colour data in `GaussianSplatData` is linear RGB. PLY/SPZ DC colour terms are converted from display RGB to linear RGB during decode; otherwise splats look washed out/milky in the native renderer, especially on Metasiberia's bright world background.

Gaussian render tuning is stored in `WorldObject::content` as:

```text
gaussian_splat_settings_v1
opacity_multiplier=1.35
brightness=1
radius_multiplier=1
```

This keeps Gaussian splats compatible with the existing server protocol: the object remains a normal generic world object, but the native client recognises the Gaussian model URL and applies the specialised editor/render settings.

The CEF converter is not a visible browser and does not create a WebView object. It loads the packaged converter from:

```text
data/resources/gaussian_splat/gaussian_splat_converter.js
data/resources/gaussian_splat/webp.wasm
```

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
- branch `feature/gaussian-splat-native-web`, commit message `feat(client): add Gaussian splat editor tuning`;
- `gui_client.exe` SHA-256: `7BE29116C9D35DD2730C44DBF667CA7AF4EA0067C50ED56B44F622DB1D30404F`;
- local Windows `server` target SHA-256: `715E5B0FEB77E641DA08653B86D3D33C047E36C3160C13BD55836FE9C4FB0823`;
- Emscripten output: `C:\programming\substrata_output_emscripten_gaussian\test_builds\gui_client.js/.wasm/.data`.

## Production Rule

Production Metasiberia runs a Linux ELF `server` on `metasiberia-server`, not Windows `server.exe`. This branch has only been built locally. Do not deploy or restart production services without an explicit current confirmation from the owner.

For production rollout, first build Linux `server` using the documented `/srv/metasiberia/build/master` workflow, stage a release directory, then restart and verify `7600/8080/8443`.
