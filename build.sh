#!/bin/bash

set -euo pipefail

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BUILD_DIR="$ROOT_DIR/_build"
MESON_ARGS=("$@")

"$ROOT_DIR/tools/bootstrap-venv.sh"

if [[ -d "$BUILD_DIR" ]]; then
  PATH="$ROOT_DIR/.venv/bin:$PATH" meson setup --reconfigure "$BUILD_DIR" "${MESON_ARGS[@]}"
else
  PATH="$ROOT_DIR/.venv/bin:$PATH" meson setup "$BUILD_DIR" "${MESON_ARGS[@]}"
fi
PATH="$ROOT_DIR/.venv/bin:$PATH" meson compile -C "$BUILD_DIR"
