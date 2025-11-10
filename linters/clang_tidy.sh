#!/bin/bash

set -euo pipefail

CLANG_TIDY=$(which clang-tidy)

if [ ! -x "$CLANG_TIDY" ]; then
	echo "clang-tidy: not found"
	exit 1
fi

usage() {
	echo "Usage: $0 {check|fix} \"-Wall -Wextra -Werror\" \"-Iinclude -Iotherinclude\" sources tests"
	echo "	{check|fix} - mode selection"
	echo "	\"-Wall -Wextra -Werror\" - compile flags of your application"
	echo "	\"-Iinclude -Iotherinclude\" - link flags of your application"
	echo "	sources tests - folders with sources to check (no spaces in folder names)"
	exit 1
}

if [ $# -lt 4 ]; then
	usage
fi

COMPILE_FLAGS=$2
LINK_FLAGS=$3
SOURCES=("${@:4}")

# translation units
TUS=$(find "${SOURCES[@]}" -type f -name '*.cpp' | sort)

SETTINGS_FILE_FLAG="--config-file=linters/.clang-tidy"

# ignore messages like 'warning: no header providing "pollfd" is directly included [misc-include-cleaner]'.
# it doesn't make sense to include every single system item directly,
# since they can be declared in internal headers with different names in different OSes.
# we do include the headers directly if they are standard though,
# so for std::string please add #include <string>
CLANG_TIDY_IGNORED_REGEX="SOL_SOCKET|SO_REUSEADDR|POLLIN|poll|pollfd|__sig_atomic_t"

check() {
	echo "Running clang-tidy check..."
	errs=0
	for f in "${TUS[@]}"; do
		out=$(mktemp)
		"$CLANG_TIDY" $f "$SETTINGS_FILE_FLAG" -- "$COMPILE_FLAGS" "$LINK_FLAGS" >$out 2>&1 || true
		filtered=$(grep -n -E 'warning:|error:' "$out" | grep -v -E "$CLANG_TIDY_IGNORED_REGEX" || true)
		if [ -n "$filtered" ]; then
			printf "%s\n" "$filtered"
			errs=1
		fi
		rm -f $out
	done
	if [ $errs -ne 0 ]; then
		exit 1
	else
		echo "clang-tidy: no issues"
	fi
}

fix() {
	for f in "${TUS[@]}"; do
		"$CLANG_TIDY" -fix "$SETTINGS_FILE_FLAG" $f -- "$COMPILE_FLAGS" "$LINK_FLAGS" || true; \
	done
}

case "$1" in
	check) check ;;
	fix) fix ;;
	*)
		usage
		;;
esac