#!/bin/bash

# Compile all .c files found in a src directory into one binary
# Usage: ./compile_src.sh [path_to_src] [output_binary]

set -euo pipefail

SRC_DIR="${1:-src}"
OUT_BIN="${2:-app}"
CC="${CC:-gcc}"

if [ ! -d "$SRC_DIR" ]; then
    echo "Error: source directory '$SRC_DIR' not found." >&2
    exit 1
fi

# Collect all .c files recursively, null-delimited for safety
files=()
while IFS= read -r -d '' file; do
    files+=("$file")
done < <(find "$SRC_DIR" -type f -name "*.c" -print0)

if [ "${#files[@]}" -eq 0 ]; then
    echo "No .c files found in '$SRC_DIR'." >&2
    exit 1
fi

echo "Found ${#files[@]} source file(s):"
printf '  %s\n' "${files[@]}"

echo "Compiling into: $OUT_BIN"
"$CC" "${files[@]}" -o "$OUT_BIN" -lraylib -lm

echo "----------------------------------------"
echo "Build succeeded: $OUT_BIN"
