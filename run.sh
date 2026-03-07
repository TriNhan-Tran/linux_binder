#!/bin/bash
#
# Run all IPC processes in the correct order (no build step).
#
# Usage: ./run.sh
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Ensure binderfs is mounted and device permission is prepared.
bash "${SCRIPT_DIR}/mount.sh"

require_binary() {
    local bin_path="$1"
    if [[ ! -x "$bin_path" ]]; then
        echo "[!] Missing binary: $bin_path"
        echo "[!] Build first with: ./build.sh"
        exit 1
    fi
}

require_binary "$BUILD_DIR/srv_manager"
require_binary "$BUILD_DIR/led_srv"
require_binary "$BUILD_DIR/audio_srv"
require_binary "$BUILD_DIR/app"

echo "=========================================="
echo " IPC Binder Demo - 4 Process Architecture"
echo "=========================================="
echo ""

# Cleanup on exit
SM_PID=""
LED_PID=""
AUDIO_PID=""

cleanup() {
    echo ""
    echo "[*] Cleaning up..."
    if [[ -n "$SM_PID" ]]; then kill "$SM_PID" 2>/dev/null || true; fi
    if [[ -n "$LED_PID" ]]; then kill "$LED_PID" 2>/dev/null || true; fi
    if [[ -n "$AUDIO_PID" ]]; then kill "$AUDIO_PID" 2>/dev/null || true; fi
    if [[ -n "$SM_PID" ]]; then wait "$SM_PID" 2>/dev/null || true; fi
    if [[ -n "$LED_PID" ]]; then wait "$LED_PID" 2>/dev/null || true; fi
    if [[ -n "$AUDIO_PID" ]]; then wait "$AUDIO_PID" 2>/dev/null || true; fi
    echo "[*] Done."
}
trap cleanup EXIT

# 1. Start SrvManager (must be first - it's the context manager)
echo "[1/4] Starting SrvManager..."
"$BUILD_DIR/srv_manager" &
SM_PID=$!
sleep 1

# 2. Start LedSrv
echo "[2/4] Starting LedSrv..."
"$BUILD_DIR/led_srv" &
LED_PID=$!
sleep 0.5

# 3. Start AudioSrv
echo "[3/4] Starting AudioSrv..."
"$BUILD_DIR/audio_srv" &
AUDIO_PID=$!
sleep 0.5

# 4. Start App (foreground - runs the test sequence)
echo "[4/4] Starting App..."
echo ""
"$BUILD_DIR/app"

echo ""
echo "[*] App finished. Shutting down services..."
