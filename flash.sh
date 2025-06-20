#!/bin/bash
set -e
DIR="base/build"
make -C "$DIR" clean
make -C "$DIR" all
if command -v commander >/dev/null; then
    commander flash "$DIR/base.hex"
else
    echo "commander tool not found. Please install the Silicon Labs commander CLI." >&2
fi
