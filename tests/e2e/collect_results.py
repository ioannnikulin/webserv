import json
import sys

total = 0
passed = 0
failed = 0

for path in sys.argv[1:]:
    with open(path) as f:
        data = json.load(f)

    for test in data:
        total += 1
        if data[test]["ok"]:
            passed += 1
        else:
            failed += 1

summary = {
    "total": total,
    "passed": passed,
    "failed": failed,
}

print("End-to-end testing summary:")
print(json.dumps(summary, indent=2))

if failed > 0:
    sys.exit(1)