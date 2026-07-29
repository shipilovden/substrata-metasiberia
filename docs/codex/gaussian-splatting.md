# Gaussian Splatting in Metasiberia

Статус: **implemented on `feature/gaussian-splat-native-web`; local Qt5, web/Emscripten and Windows server builds verified on 2026-07-29.**

## What Works

Metasiberia now treats Gaussian splats as first-class world objects, not as a browser-only prototype.

- Native Qt5 client loads and renders Gaussian splats in the normal 3D scene.
- Web/Emscripten client has an ImGui "Gaussian splats" upload panel.
- Uploaded splats become ordinary editable world objects: select them, then use the existing transform controls for position, rotation and scale.
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
- `gui_client.exe` SHA-256: `FFFDA9AD1A235BD2FA4D1303E9258B73DC00FBFB26D7F5701E21BA5B6BE73AA6`;
- local Windows `server` target SHA-256: `7EF6BA0ADFF8264A94B1B4586231797B1D5E02D25236E930E25A2872BA371BCE`;
- Emscripten output: `C:\programming\substrata_output_emscripten_gaussian\test_builds\gui_client.js/.wasm/.data`.

## Production Rule

Production Metasiberia runs a Linux ELF `server` on `metasiberia-server`, not Windows `server.exe`. This branch has only been built locally. Do not deploy or restart production services without an explicit current confirmation from the owner.

For production rollout, first build Linux `server` using the documented `/srv/metasiberia/build/master` workflow, stage a release directory, then restart and verify `7600/8080/8443`.
