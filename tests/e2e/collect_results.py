import json
import sys
from pathlib import Path

total = 0
passed = 0
failed = 0
leaked = 0
fd_problems = 0
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
                toPrint.append(f"Test '{data[test]['name']}' failed:\n{data[test].get('error', 'no error')}\nexpected status: {data[test].get('expected_status')}\ngot status: {data[test].get('actual_status')}\nexpected body: {data[test].get('expected_body', 'none')}\ngot body: {data[test].get('actual_body', 'none')}")
    elif p.suffix == ".log":
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            print(f"reading {path}")
            content = f.read()
            if "Using Valgrind" in content:
                if "All heap blocks were freed -- no leaks are possible" not in content:
                    leaked += 1
                    toPrint.append(f"Memory leaks detected in log file: {path}")
                if "FILE DESCRIPTORS: 3 open (3 std) at exit" not in content:
                    fd_problems += 1
                    toPrint.append(f"Unclosed file descriptors detected in log file: {path}")

summary = {
    "total": total,
    "passed": passed,
    "failed": failed,
}

print("End-to-end testing summary:")
print(json.dumps(summary, indent=2))
if toPrint:
    print("\nErrors:\n")
    print("\n\n".join(toPrint))

if failed > 0 or leaked > 0 or fd_problems > 0:
    sys.exit(1)