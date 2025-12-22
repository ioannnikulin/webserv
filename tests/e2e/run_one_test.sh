#!/bin/bash

set -e

TESTDIR="$1"
PROJECT_NAME="$(basename "$TESTDIR")"

RESULTS_DIR="tests/e2e/$TESTDIR/results"
mkdir -p "$RESULTS_DIR"

echo "Running test suite: $PROJECT_NAME"

# when tester0 ends, everything ends, so for simultaneous connections make sure to assign the longest scenario to him
docker compose -p "$PROJECT_NAME" -f "tests/e2e/$TESTDIR/docker-compose.yml" up \
    --abort-on-container-exit tester0

GLOBAL_RESULTS_DIR="tests/e2e/results"
mkdir -p "${GLOBAL_RESULTS_DIR}"/"${PROJECT_NAME}"

cp "$RESULTS_DIR"/* "${GLOBAL_RESULTS_DIR}"/"${PROJECT_NAME}"/

docker compose -p "$PROJECT_NAME" -f "tests/e2e/$TESTDIR/docker-compose.yml" down -v > /dev/null 2>&1 || true

echo "Results of test suite $PROJECT_NAME saved to $GLOBAL_RESULTS_DIR"
