# Codex Piano VST3 (JUCE)

Piano-style synth instrument plugin built with JUCE, based on the same CMake-first structure as `Low-End-VST`.

## Features
- VST3 + Standalone targets
- MIDI input
- 16-voice polyphony
- Gain, Brightness, Release, Reverb controls
- No external sample library required (synthesized piano-like timbre)

## Project Layout
- `CMakeLists.txt`
- `Source/PluginProcessor.*`
- `Source/PluginEditor.*`
- `Assets/`
- `scripts/`

## Build (Linux/macOS/WSL)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Build (Windows Visual Studio 2022)
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

## Artifacts
- `build/CodexPianoVST3_artefacts/VST3/Codex Piano VST3.vst3`
- `build/CodexPianoVST3_artefacts/Standalone/`

## Optional Packaging
- Linux tarball: `./scripts/package_linux_tar.sh build`
- macOS dmg: `./scripts/package_macos_dmg.sh build`
- Windows zip: `powershell -ExecutionPolicy Bypass -File scripts/package_windows_zip.ps1 build`

## GitHub Windows Release Workflow
- Workflow file: `.github/workflows/windows-release.yml`
- Triggers:
  - Manual: `workflow_dispatch`
  - Tag push: `v*` (example: `v0.1.0`)
- What it does:
  - Builds `CodexPianoVST3_VST3` and `CodexPianoVST3_Standalone` on `windows-latest`
  - Packages `build/packages/CodexPianoVST3-windows.zip`
  - Uploads the zip as a workflow artifact
  - Creates a GitHub Release and attaches the zip when the ref is a `v*` tag

## Next Upgrade
For realistic piano tone, replace the current voice synthesis with layered sampled notes via `juce::SamplerSound`.
