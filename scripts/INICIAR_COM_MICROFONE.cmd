@echo off
setlocal
cd /d "%~dp0Jogo"
if not exist "soh-microfone-v0.1.4.exe" (
  echo Executavel do mod ausente. Extraia o pacote preservando as pastas.
  pause
  exit /b 1
)
if not exist "soh.o2r" (
  echo Falta a instalacao oficial do Ship of Harkinian 9.3.1 em Jogo.
  echo Leia README.pt-BR.md e configure OpenGL antes de executar.
  pause
  exit /b 1
)
set "TRIAEVUM_OCARINA_MIC=1"
echo Ocarina por microfone - N64 no PC v0.1.4 experimental
"soh-microfone-v0.1.4.exe"
if not "%errorlevel%"=="0" pause
endlocal
