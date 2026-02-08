param(
  [string]$BuildDir = "build"
)

$artefacts = Join-Path $BuildDir "CodexPianoVST3_artefacts"
if (!(Test-Path $artefacts)) {
  Write-Error "Artefacts directory not found: $artefacts"
  exit 1
}

$vst3Dir = Join-Path $artefacts "VST3"
$standaloneDir = Join-Path $artefacts "Standalone"

if (!(Test-Path $vst3Dir) -or !(Test-Path $standaloneDir)) {
  $releaseDir = Join-Path $artefacts "Release"
  $vst3Dir = Join-Path $releaseDir "VST3"
  $standaloneDir = Join-Path $releaseDir "Standalone"
}

if (!(Test-Path $vst3Dir)) {
  Write-Error "VST3 directory not found: $vst3Dir"
  exit 1
}

if (!(Test-Path $standaloneDir)) {
  Write-Error "Standalone directory not found: $standaloneDir"
  exit 1
}

$outDir = Join-Path $BuildDir "packages"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$zipPath = Join-Path $outDir "CodexPianoVST3-windows.zip"
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }

Compress-Archive -Path $vst3Dir, $standaloneDir -DestinationPath $zipPath -Force
Write-Output "Created: $zipPath"
