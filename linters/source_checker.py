#!/usr/bin/env python3

import sys
import re
from pathlib import Path
from header_checker import checkCommentPrefixes
from header_checker import checkAST
from header_checker import lineNum

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
    return (sorted(files))

regexpUsingNamespace = re.compile(r"^\s*using\snamespace\s", re.M)
regexpBad_ReturnNoSpaceOrParens = re.compile(
    # fail for `return;` 	(first part; only space after return),
	# `return(` 			(same),
	# `return x` 			(second part: after a space only `;` or `)` allowed).
    # allowed: `return ;` `return (x);`
    r"^\s*return(([^ ])|( [^\\(;]))", re.M
)
regexpPoll = re.compile(f"^bpoll\\(")

files = collectFiles(roots)
if not files:
    print("No source files found.")
    sys.exit(0)

failed = False
pollCalls = 0

for f in files:
    s = f.read_text()

    issues = []

    for match in regexpBad_ReturnNoSpaceOrParens.finditer(s):
        issues.append(lineNum(match, s) + "please wrap returned values in parenthesis, preceeded by a space. On returning void use `return ;`")

    if regexpUsingNamespace.search(s):
        issues.append(lineNum(match, s) + "contains 'using namespace' directive; please switch to explicit directive like 'using std::string' etc.")

    checkCommentPrefixes(s, issues)

    if regexpPoll.search(s):
        pollCalls += 1

    issues += checkAST(f)

    if issues:
        failed = True
        print(f"{f}:\n" + "\n\t".join(issues))

if pollCalls > 1:
    failed = True
    print("too many poll() calls detected")

if failed:
    sys.exit(1)
else:
    print("source checker didn't find any issues")
