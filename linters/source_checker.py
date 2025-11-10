#!/usr/bin/env python3

import sys
import re
from pathlib import Path

roots = sys.argv[1:] if len(sys.argv) > 1 else ["sources", "include", "tests"]
headerSuffixes = {".cpp", ".c"}

def collectFiles(roots):
    files = []
    for r in roots:
        p = Path(r)
        if p.is_file() and p.suffix in headerSuffixes:
            files.append(p)
        elif p.is_dir():
            files += [f for f in p.rglob("*") if f.suffix in headerSuffixes]
    return sorted(files)

regexpUsingNamespace = re.compile(r"^\s*using\snamespace\s", re.M)
regexpReturnNoSpaceOrParens = re.compile(r"^\s*return(([^ ])|( [^\\(]))", re.M)

files = collectFiles(roots)
if not files:
    print("No source files found.")
    sys.exit(0)

failed = False
for f in files:
    s = f.read_text()

    issues = []

    if regexpReturnNoSpaceOrParens.search(s):
        issues.append("please wrap returned values in parenthesis, preceeded by a space")

    if regexpUsingNamespace.search(s):
        issues.append("contains 'using namespace' directive; please switch to explicit directive like 'using std::string' etc.")

    if issues:
        failed = True
        print(f"{f}: " + "; ".join(issues))

if failed:
    sys.exit(1)