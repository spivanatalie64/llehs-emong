#!/bin/bash

set -euo pipefail

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
VENV_DIR="$ROOT_DIR/.venv"

if [[ ! -d "$VENV_DIR" ]]; then
  python3 -m venv "$VENV_DIR"
fi

"$VENV_DIR/bin/python" -m pip install --upgrade pip libsass

cat >"$VENV_DIR/bin/sassc" <<'EOF'
#!/usr/bin/env python3

import argparse
import pathlib
import sys

import sass


def main() -> int:
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("-a", action="store_true")
    parser.add_argument("input")
    parser.add_argument("output")
    args = parser.parse_args()

    css = sass.compile(filename=args.input, output_style="expanded")
    pathlib.Path(args.output).write_text(css)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
EOF

chmod +x "$VENV_DIR/bin/sassc"
