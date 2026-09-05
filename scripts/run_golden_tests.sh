#!/usr/bin/env bash
set -euo pipefail

KWIC_BIN="${1:?Usage: $0 <path-to-kwic-binary>}"
WIDTH=40
METHODS=(shared_data abstract_data implicit_invocation filters)

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="$ROOT/tests"
GOLDEN_DIR="$TEST_DIR/golden"

status=0

for input in "$TEST_DIR/small" "$TEST_DIR/long"; do
    name="$(basename "$input")"
    golden="$GOLDEN_DIR/${name}.width${WIDTH}.txt"

    if [[ ! -f "$golden" ]]; then
        echo "MISSING golden file: $golden" >&2
        status=1
        continue
    fi

    for method in "${METHODS[@]}"; do
        if diff -u "$golden" <("$KWIC_BIN" --method "$method" --file "$input" --width "$WIDTH"); then
            echo "OK    $method  $name"
        else
            echo "FAIL  $method  $name" >&2
            status=1
        fi
    done
done

exit "$status"
