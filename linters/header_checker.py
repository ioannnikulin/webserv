#!/usr/bin/env python3

import sys
import re
from pathlib import Path

roots = sys.argv[1:] if len(sys.argv) > 1 else ["sources", "include", "tests"]
headerSuffixes = {".h", ".hpp", ".hh", ".hxx"}

def collectFiles(roots):
    files = []
    for r in roots:
        p = Path(r)
        if p.is_file() and p.suffix in headerSuffixes:
            files.append(p)
        elif p.is_dir():
            files += [f for f in p.rglob("*") if f.suffix in headerSuffixes]
    return sorted(files)

regexpPragma = re.compile(r"^\s*#\s*pragma\s+once\b", re.M)
regexpUsing = re.compile(r"^\s*using\s", re.M)
regexpClass = re.compile(r'\bclass\s+(\w+)\b')
regexpConstructor = re.compile(r'\b{cls}\s*\(')
regexpDestructor = re.compile(r'~{cls}\s*\(')
regexpCopyConstructor = re.compile(r'\b{cls}\s*\(\s*const\s+{cls}\s*&')
regexpCopyAssignment = re.compile(r'operator=\s*\(\s*const\s+{cls}\s*&')

files = collectFiles(roots)
if not files:
    print("No header files found.")
    sys.exit(0)

failed = False
for f in files:
    s = f.read_text()
    hasPragma = bool(regexpPragma.search(s))

    issues = []

    for match in regexpClass.finditer(s):
        cls = match.group(1)
        patterns = {
            'constructor': regexpConstructor.pattern.format(cls=cls),
            'destructor': regexpDestructor.pattern.format(cls=cls),
            'copy constructor': regexpCopyConstructor.pattern.format(cls=cls),
            'copy assignment': regexpCopyAssignment.pattern.format(cls=cls),
        }
        missing = []
        for name, pattern in patterns.items():
            if not re.search(pattern, s):
                missing.append(name)
        if missing:
            issues.append(f"{f}:{cls} — missing {', '.join(missing)}")

    # disabled for now, sticking with traditional header guards
    #if not hasPragma:
    #    issues.append("missing #pragma once")

    if regexpUsing.search(s):
        issues.append("contains 'using' directive (please use fully qualified names in headers; you can use `using` in source files though)")

    if issues:
        failed = True
        print(f"{f}: " + "; ".join(issues))

if failed:
    sys.exit(1)