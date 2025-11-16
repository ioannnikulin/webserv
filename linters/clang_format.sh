#!/bin/bash

set -euo pipefail

usage() {
	echo "Usage: $0 {check|fix} sources tests"
	echo "	{check|fix} - mode selection"
	echo "	sources tests - folders with sources to check (no spaces in folder names)"
	exit 1
}

if [ $# -lt 2 ]; then
	usage
fi

SOURCES=("${@:2}")

CLANG_FORMAT=clang-format

collect() {
	mapfile -d '' FILES < <(find "${SOURCES[@]}" -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0)
}

SETTINGS_FILE_FLAG="-style=file:linters/.clang-format"

# run to fix simple formatting issues
# see .clang-format for settings
fix() {
	echo "Running clang-format autoformatting..."
	for f in "${FILES[@]}"; do
		"$CLANG_FORMAT" "$SETTINGS_FILE_FLAG" -i "$f"
	done
	echo "clang-format autoformat applied"
}

# runs fixer and compares with original,
# exits with 1 if any file would change
check() {
	echo "Running clang-format check..."
	changed=0
	for f in "${FILES[@]}"; do
		tmp=$f.formatted.tmp
		"$CLANG_FORMAT" "$SETTINGS_FILE_FLAG" "$f" > "$tmp"
		if ! cmp -s "$f" "$tmp"; then
			echo "$f is malformatted"
			changed=1
		fi
		rm -f "$tmp"
	done
	if [ "$changed" -eq 0 ]; then
		echo "clang-format: no issues"
	fi
	exit $changed
}

collect
case "$1" in
	check) check ;;
	fix) fix ;;
	*)
		usage
		;;
esac