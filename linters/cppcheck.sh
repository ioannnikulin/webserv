#!/bin/bash

set -euo pipefail

CPPCHECK=$(which cppcheck)

if [ ! -x "$CPPCHECK" ]; then
	echo "cppcheck: not found"
	exit 1
fi

usage() {
	echo "Usage: $0 c++98 \"-Iinclude -Iotherinclude\" sources tests"
	echo "	c++98 - language standard"
	echo "	\"-Iinclude -Iotherinclude\" - link flags of your application"
	echo "	sources tests - folders with sources to check (no spaces in folder names)"
	exit 1
}

if [ $# -lt 3 ]; then
	usage
fi

LANG_STD="$1"
INCLUDES="$2"
SRC_DIRS=("${@:3}")

collect() {
    mapfile -d '' FILES < <(find "${SRC_DIRS[@]}" -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0)
}

# without this it complains about every field in a class declaration
# that is not used immediately in the same header
NO_UNUSED_FIELDS_IN_HEADERS="--suppress=unusedStructMember:*.hpp"
# some are used in tests, which are generated on the fly and are not analyzed by cppcheck
NO_UNUSED_FUNCTIONS="--suppress=unusedFunction/*"

check() {
	echo "Running cppcheck..."
	out=$(mktemp)
	"$CPPCHECK" --std="$LANG_STD" --enable=all --inconclusive \
		"$NO_UNUSED_FIELDS_IN_HEADERS" \
		"$NO_UNUSED_FUNCTIONS" \
		"$INCLUDES" "${FILES[@]}" \
		--template='{file}:{line}:{column}:{severity}:{id}:{message}' >"$out" 2>&1 || true

	# functions in test folder are not necessarily unused, they are picked up by cxxtest generator later
	grep -E ':(error|warning|style):' "$out" | grep -v -E 'tests/.*:.*:.*:.*:unusedFunction:' > "$out.filtered" || true

	if [ -s "$out.filtered" ]; then
		echo "cppcheck found issues:"
		cat "$out.filtered"
		rm -f "$out" "$out.filtered"
		echo "-------------------------------------------------"
		exit 1
	else
		echo "cppcheck: no issues"
		rm -f "$out" "$out.filtered"
		echo "-------------------------------------------------"
	fi
}

collect
check
