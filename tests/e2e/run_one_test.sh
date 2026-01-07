#!/bin/bash

set -e

TESTDIR="$1"
PROJECT_NAME="$(basename "$TESTDIR")"

REQ_DIR="tests/e2e/$TESTDIR/requirements"

for node in "$REQ_DIR"/*; do
    [ -d "$node" ] || continue
    mkdir -p "$node/logs"
    mkdir -p "$node/results"
done

echo "Running test suite: $PROJECT_NAME"

# when tester0 ends, everything ends, so for simultaneous connections make sure to assign the longest scenario to him
UID="$(id -u)" GID="$(id -g)" docker compose -p "$PROJECT_NAME" -f "tests/e2e/$TESTDIR/docker-compose.yml" up \
    --abort-on-container-exit tester0

docker compose -p "$PROJECT_NAME" -f "tests/e2e/$TESTDIR/docker-compose.yml" down -v > /dev/null 2>&1 || true
