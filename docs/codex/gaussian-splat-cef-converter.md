# Hidden CEF Gaussian conversion

`gui_client/GaussianSplatCEFConverter.*` is the native Qt5 fallback for
Gaussian containers that `GaussianSplatDecoder` does not decode directly.
It does not open a browser window and it does not create a world WebView.
It runs the self-hosted `@playcanvas/splat-transform` bundle in a 1×1
off-screen CEF browser, normalises the source to an ordinary
`binary_little_endian` PLY and returns both its temporary path and bytes.

The canonical direct path remains:

- standard 3DGS binary PLY -> `GaussianSplatDecoder`;
- 32-byte `.splat` -> `GaussianSplatDecoder`;
- `.spz` -> `GaussianSplatDecoder`.

Use the hidden converter for compressed PLY, KSPLAT, SOG, LCC and LCC2,
then pass `outputBytes()` to:

```cpp
GaussianSplatDecoder::decode("metasiberia-runtime.ply", ArrayRef<uint8>(...));
```

## GUI integration

The owner (normally `AddObjectDialog` or a small controller owned by
`GUIClient`) keeps a `Reference<GaussianSplatCEFConverter>` alive:

```cpp
GaussianSplatCEFConverter::Config config;
config.input_path = selected_path;
config.converter_script_path =
	GaussianSplatCEFConverter::packagedConverterScriptPath(resources_dir_path);
config.webp_wasm_path =
	GaussianSplatCEFConverter::packagedWebPWasmPath(resources_dir_path);
config.output_path = unique_temp_path_ending_in_ply;

converter = new GaussianSplatCEFConverter();
converter->start(config);
```

The normal GUI tick calls `converter->think()`.  While it is running,
`progressStage()` (`read`, `convert`, `ready`) and `progressValue()` can drive
the dialog's progress text/bar. When `isFinished()` becomes true:

- `State_Succeeded`: decode `outputBytes()` (or mmap `outputPath()`), then
  release the converter;
- `State_Failed`: show `errorMessage()`, delete any caller-owned partial
  temporary output and release the converter.

`CEF::doMessageLoopWork()` is already pumped by the client. Do not call it
from this class or from a worker thread. Conversion start, `think()`, result
inspection and destruction must stay on that same UI thread.

If built with `CEF_SUPPORT=0`, `start()` moves to `State_Failed` with a clear
diagnostic and performs no browser work.

## Related files and security

SOG/LCC metadata may request sidecars. By default, the converter exposes the
selected file's directory at its private virtual `/input/` origin. Every
request is:

1. URL-decoded and rejected if it is absolute or contains `..`;
2. resolved to an existing file;
3. canonicalised;
4. accepted only if its canonical path is still below the selected input
   directory.

This also blocks symlink/junction escapes. Explicit sidecars may be supplied
in `Config::related_files`, but they pass the same canonical directory guard.
Only the converter JS and WebP WASM have dedicated routes; the virtual origin
cannot navigate to the network.

`scripts/copy_files_to_output.rb` installs the audited web bundle into:

```text
data/resources/gaussian_splat/gaussian_splat_converter.js
data/resources/gaussian_splat/webp.wasm
```
