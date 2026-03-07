#!/bin/bash
#
# Configure and build the project.
#
# Usage: ./build.sh
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

echo "[*] Configuring project..."
cmake -B "$BUILD_DIR" -S "$SCRIPT_DIR" -DCMAKE_BUILD_TYPE=Debug

echo "[*] Building project..."
cmake --build "$BUILD_DIR" -j"$(nproc)"

echo "[*] Build complete."
