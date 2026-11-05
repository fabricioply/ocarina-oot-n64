$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$upstreamPath = Join-Path $repoRoot 'upstream/Shipwright'
$patchPath = Join-Path $repoRoot 'patches/shipwright-9.3.1-microphone.patch'
$expected = 'f0599f96f8d442f26d0fa1c5cedafddacd56633f'
$actual = & git -C $upstreamPath rev-parse HEAD
if ($LASTEXITCODE -ne 0 -or $actual -ne $expected) { throw 'Initialize the pinned Shipwright submodule first.' }
$ErrorActionPreference = 'Continue'
& git -C $upstreamPath apply --reverse --check $patchPath 2>$null
$reverseCode = $LASTEXITCODE
$ErrorActionPreference = 'Stop'
if ($reverseCode -eq 0) { Write-Output 'Microphone patch already applied.'; exit 0 }
& git -C $upstreamPath apply --check $patchPath
if ($LASTEXITCODE -ne 0) { throw 'Patch check failed; no changes applied.' }
& git -C $upstreamPath apply $patchPath
if ($LASTEXITCODE -ne 0) { throw 'Patch apply failed.' }
Write-Output 'Microphone patch applied.'
