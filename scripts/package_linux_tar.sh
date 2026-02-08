#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:-build}"
ARTEFACTS_DIR="$BUILD_DIR/CodexPianoVST3_artefacts"
OUT_DIR="$BUILD_DIR/packages"

mkdir -p "$OUT_DIR"

if [ ! -d "$ARTEFACTS_DIR" ]; then
  echo "Artefacts directory not found: $ARTEFACTS_DIR"
  exit 1
fi

TARBALL="$OUT_DIR/CodexPianoVST3-linux.tar.gz"
tar -czf "$TARBALL" -C "$ARTEFACTS_DIR" VST3 Standalone 2>/dev/null || tar -czf "$TARBALL" -C "$ARTEFACTS_DIR" VST3

echo "Created: $TARBALL"
