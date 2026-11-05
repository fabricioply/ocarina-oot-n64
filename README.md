# Ocarina microphone mod — OoT N64 on PC

Play monophonic melodies on an instrument, hum or whistle into a microphone to control the ocarina in **Ship of Harkinian 9.3.1, Windows x64**.

[Português](README.pt-BR.md) · [Download v0.1.4](https://github.com/fabricioply/ocarina-oot-n64/releases/tag/v0.1.4)

**Experimental prerelease:** 11 automated test groups pass. v0.1.4 fixes listening incorrectly blocked by ImGui mouse/keyboard capture over the game viewport. Full recognition through native song execution still needs live validation after this fix. This is not a stable, fully verified release.

This is the original N64 game on PC. It is not OoT 3D, an emulator plugin or support for a physical N64.

## Features

- Assisted interval matching of the 12 standard songs; the whole melody can be transposed.
- Monophonic instruments, humming and whistling. No chords.
- Native in-game Yes/No questions for ambiguous phrases.
- Microphone selection and level meter: **Esc > Settings > Audio > Microfone da Ocarina**.
- Native controller-command replay; the game's learned-song and location requirements apply.
- Local processing, no audio recording or mod network traffic. Logs contain derived measurements.

## Install

1. Prepare the official [Ship of Harkinian 9.3.1](https://github.com/HarbourMasters/Shipwright/releases/tag/9.3.1) Windows release using your own supported game dump. Confirm the original game works.
2. **Choose OpenGL in the original game's graphics settings and close it.** Direct3D 11 is known to crash in this experimental custom build.
3. Put the official installation files under `Jogo/`, then extract the mod zip into the parent directory:

```text
PC-OoT-N64/
  INICIAR_COM_MICROFONE.cmd
  Jogo/
    soh.exe                 # your existing official install
    soh.o2r                 # from the official install
    oot.o2r                 # generated locally from your own dump
    soh-microfone-v0.1.4.exe # from this release
```

The other official installation files must remain in `Jogo/`. This zip only provides the custom executable, launcher and documentation/licenses.

4. Run `INICIAR_COM_MICROFONE.cmd`. Choose the microphone in the menu, close the menus, equip and take out the ocarina. Play the entire phrase and finish with a short pause.
5. For native questions: **A or C-Up** confirms; **No** offers the next candidate; **B** cancels. Release the buttons before resuming.

Back up your saves. The mod shares `Jogo/Save` with the original executable. Microphone settings are generated locally in `Jogo/config/ocarina_microfone_n64.json`; no personal settings are distributed.

## Limits and diagnostics

- Windows x64/OpenGL smoke-tested. No Linux, macOS or console build supplied.
- Live instrument, voice and whistle recognition still needs refinement and testing.
- No chords, custom Scarecrow's Song, teaching/recording or ocarina minigames.
- A manual note command cancels listening until the ocarina is put away and opened again.
- Output audio is muted during listening to limit feedback, preserving saved volume settings.
- Logs: `Jogo/microfone-notas-v0.4.3.log` (legacy filename) and `Jogo/logs/Ship of Harkinian.log`. Logs can contain local device identifiers; review before sharing.

## Source and build

This repository includes the mod, tests and exact upstream integration patch. Shipwright is pinned as a submodule. See [BUILDING.md](docs/BUILDING.md), [UPSTREAM.json](UPSTREAM.json) and [CHANGELOG.md](CHANGELOG.md).

```powershell
git clone --recurse-submodules https://github.com/fabricioply/ocarina-oot-n64.git
cd ocarina-oot-n64
cmake -S . -B build-tests -G "Visual Studio 17 2022" -A x64
cmake --build build-tests --config Release
ctest --test-dir build-tests -C Release --output-on-failure
```

## Credits and licensing

Based on [Ship of Harkinian](https://github.com/HarbourMasters/Shipwright), libultraship and Torch. Shared recognition code comes from [ocarina-oot3d-pc](https://github.com/fabricioply/ocarina-oot3d-pc). Physical 3DS support: [ocarina-3ds](https://github.com/fabricioply/ocarina-3ds).

Original mod sources: GPL-3.0-or-later. Upstream and third-party code retain their own licenses: see [LICENSE](LICENSE) and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). No game, ROM, extracted game archive, save or personal settings are included. Unofficial project, not affiliated with Nintendo or Harbour Masters.
