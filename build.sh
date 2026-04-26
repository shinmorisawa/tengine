#!/usr/bin/env bash

set -euo pipefail # thanks cloudflare!!!

if [ ! -d build ]; then
    AR="llvm-ar" meson setup build -Doptimization=3
fi

ninja -C build
