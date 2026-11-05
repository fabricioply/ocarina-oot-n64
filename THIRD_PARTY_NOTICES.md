# Source provenance and notices

Original microphone-mod sources, including the core copied from fabricioply/ocarina-oot3d-pc, are GPL-3.0-or-later. The executable is a modified Ship of Harkinian build. This license does not relicense upstream or third-party components.

- Shipwright 9.3.1: https://github.com/HarbourMasters/Shipwright, pinned in UPSTREAM.json and the submodule. Upstream credits, code provenance and notices remain applicable; our exact changes are in patches/.
- libultraship: MIT, licenses/libultraship-MIT.txt. Its fast rendering code: licenses/fast-MIT.txt.
- Torch: MIT, licenses/Torch-MIT.txt.
- OoT-Randomizer-derived code: licenses/OoT-Randomizer-MIT.md.
- Static Windows dependencies: notices from the build's vcpkg installation in licenses/third-party/. These cover SDL2, SDL2-net, GLEW, GLFW, zlib, bzip2, libzip, PNG, nlohmann-json, tinyxml2, spdlog/fmt, Ogg/Vorbis, Opus/opusfile and transitive dependencies. Their respective licenses apply.

The source and toolchain recipe is in docs/BUILDING.md. No Nintendo ROM, extracted game asset archive, save, key, recorded audio or personal configuration is distributed. Users must prepare the official base install and their own game data locally. This project is not affiliated with Nintendo, Harbour Masters or the upstream projects.
