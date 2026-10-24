#!/bin/sh
# Host tests for N3's connection-state logic. Needs only gcc.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
OUT=${TMPDIR:-/tmp}/n3_host_tests
mkdir -p "$OUT"
CFLAGS="-std=c11 -O2 -Wall -Wextra -Werror -I$ROOT/src -I$HERE/stubs"
gcc $CFLAGS "$HERE/test_conn_state.c" "$ROOT/src/core/conn_state.c" -o "$OUT/test_conn_state"
"$OUT/test_conn_state"

