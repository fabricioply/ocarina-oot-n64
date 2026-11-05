# Building

## Recognition tests

CMake 3.26+, Visual Studio 2022 C++ tools and Windows SDK:

```powershell
cmake -S . -B build-tests -G "Visual Studio 17 2022" -A x64
cmake --build build-tests --config Release
ctest --test-dir build-tests -C Release --output-on-failure
```

11 test groups. These do not prove live microphone recognition or native song effects.

## Native executable

Exact upstream/dependency revisions: `UPSTREAM.json`. The release used clang-cl/lld-link 23.1.5, Ninja, VS 2022 Build Tools, Windows SDK and static vcpkg dependencies. See upstream `docs/BUILDING.md` for prerequisites and local asset generation.

Clone with `--recurse-submodules`. GitHub source zip users must obtain Shipwright at the pinned commit and its submodules separately, under `upstream/Shipwright`.

Apply the exact integration patch:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/Apply-UpstreamPatch.ps1
```

The script checks the upstream commit and checks the patch before applying. It can be rerun if already applied.

Use an x64 VS developer shell with clang-cl, lld-link, Ninja, Python, Git and CMake on PATH. Set `$vcpkgRoot` to vcpkg at the documented commit, bootstrap it and install the dependencies listed in upstream CMake using triplet `x64-windows-static`. Configure:

```powershell
$repoRoot = (Get-Location).Path
cmake -S upstream/Shipwright -B build-native -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl `
  -DCMAKE_LINKER=lld-link `
  -DCMAKE_TOOLCHAIN_FILE="$vcpkgRoot/scripts/buildsystems/vcpkg.cmake" `
  -DVCPKG_ROOT="$vcpkgRoot" -DUSE_AUTO_VCPKG=OFF `
  -DVCPKG_TARGET_TRIPLET=x64-windows-static `
  -DVCPKG_MANIFEST_MODE=OFF `
  -DOCARINA_N64_SOURCE_ROOT="$repoRoot" -DSUPPRESS_WARNINGS=ON
cmake --build build-native --target soh soh-torch --parallel 8
```

In this configuration Shipwright writes `soh.exe` under its `x64/Release/` directory. Use it alongside a working official installation with its resources. The patch includes clang-cl warning/runtime and ThinLTO link workarounds. This is a source recipe, not a claim of bit-for-bit reproducibility across toolchains.

No ROM is needed for recognition tests. Running requires locally generated game assets from a supported copy. They are not distributed here. Use OpenGL for this experimental executable.
