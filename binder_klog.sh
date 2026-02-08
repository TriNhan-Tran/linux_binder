#!/bin/bash

# ===============================
# Binder kernel log live watcher
# ===============================

echo ""
echo "Press Ctrl+C to stop"
echo ""

# Highlight keyword
KEYWORDS="binder|BINDER|ioctl|error|warn|fail"

# Follow kernel log
if command -v journalctl >/dev/null 2>&1; then
    echo "[*] Using journalctl (kernel ring buffer)"
    sudo journalctl -kf | grep -iE --line-buffered "$KEYWORDS"
else
    echo "[*] Using dmesg"
    sudo dmesg -w | grep -iE --line-buffered "$KEYWORDS"
fi
