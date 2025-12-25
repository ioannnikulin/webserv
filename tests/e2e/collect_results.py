import json
import sys
from pathlib import Path

total = 0
passed = 0
failed = 0
leaked = 0
toPrint = []

for path in sys.argv[1:]:
    p = Path(path)
    if p.suffix == ".json":
        with open(path) as f:
            data = json.load(f)

        for test in data:
            total += 1
            if data[test]["ok"]:
                passed += 1
            else:
                failed += 1
                toPrint.append(f"Test '{data[test]['name']}' failed:\n{data[test].get('error', 'no error')}\nexpected: {data[test].get('expected_body', 'none')}\ngot: {data[test].get('actual_body', 'none')}")
    elif p.suffix == ".log":
        with open(path) as f:
            content = f.read()
            if "All heap blocks were freed -- no leaks are possible" not in content:
                leaked += 1
                toPrint.append(f"Memory leaks detected in log file: {path}")

summary = {
    "total": total,
    "passed": passed,
    "failed": failed,
}

print("End-to-end testing summary:")
print(json.dumps(summary, indent=2))
print("\nErrors:\n")
print("\n\n".join(toPrint))

if failed > 0 or leaked > 0:
    sys.exit(1)