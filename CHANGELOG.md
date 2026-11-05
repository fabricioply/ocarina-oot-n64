# Changelog

## 0.1.4 — experimental

- Fix listening blocked by ImGui mouse/keyboard capture over the viewport with menus closed.
- Explicit menus/settings, text editing, pause, transitions and cutscenes still block listening.
- Add regression coverage; 11 test groups pass. Full native song execution still needs live validation after the fix.

## 0.1.3 — diagnostic

- Per-second capture summaries and native game-state gate diagnostics.

## 0.1.2 — experimental

- Lower integrated energy floor to RMS 0.001; quiet-melody and noise regressions.

## 0.1.1 — experimental

- C-Up native confirmation handling, unexpected textbox closure recovery and button-release waiting.

## 0.1.0 — local prototype

- Ship of Harkinian 9.3.1 integration: assisted matching, native Yes/No, microphone settings, output muting and PadMgr controller injection.
