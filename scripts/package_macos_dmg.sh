#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:-build}"
ARTEFACTS_DIR="$BUILD_DIR/CodexPianoVST3_artefacts"
OUT_DIR="$BUILD_DIR/packages"

mkdir -p "$OUT_DIR"

if [ ! -d "$ARTEFACTS_DIR/Standalone" ]; then
  echo "Standalone artefact not found: $ARTEFACTS_DIR/Standalone"
  exit 1
fi

DMG_PATH="$OUT_DIR/CodexPianoVST3-macos.dmg"
TMP_DIR="$(mktemp -d)"
cp -R "$ARTEFACTS_DIR/Standalone" "$TMP_DIR/"

hdiutil create -volname "CodexPianoVST3" -srcfolder "$TMP_DIR" -ov -format UDZO "$DMG_PATH"
rm -rf "$TMP_DIR"

echo "Created: $DMG_PATH"
