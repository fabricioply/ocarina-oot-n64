# Shared recognition core

Pitch estimation, note tracking, assisted interval matching for 12 standard songs and controller replay state. Copied from the OoT3D PC mod and maintained independently for N64.

This directory also builds an optional Windows/WASAPI diagnostic CLI (`OcarinaMicrofone --list-devices`, `--choose`, `--noise-floor VALUE`). The CLI does not control the game.

Build tests from the repository root: see [BUILDING.md](../../docs/BUILDING.md). The CLI has its own noise-floor option; the integrated N64 session defaults to RMS 0.001. No audio recording or network communication is implemented.
