#!/bin/sh

set -e

TESTER_ID="$1"

python3 run_tests.py "$TESTER_ID" 2>&1 | tee /logs/tester.log