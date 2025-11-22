#!/usr/bin/env python3

import sys
import re
from pathlib import Path

roots = sys.argv[1:] if len(sys.argv) > 1 else ["sources", "include", "tests"]
headerSuffixes = {".h", ".hpp", ".hh", ".hxx"}

regexpUsing = re.compile(r"^\s*using\s", re.M)
regexpCommentOneLine = re.compile(r'//(.*)')
regexpCommentMultiline = re.compile(r'/\*(.*?)\*/', re.DOTALL)
regexpCommentValidContentPrefix = re.compile(r'^(( TODO [0-9]+:)|( NOTE: )|( namespace))')

def lineNum(match, s):
    start_pos = match.start()
    return "line " + str(s.count('\n', 0, start_pos + 1)) + ": "

def collectFiles(roots):
    files = []
    for r in roots:
        p = Path(r)
        if p.is_file() and p.suffix in headerSuffixes:
            files.append(p)
        elif p.is_dir():
            files += [f for f in p.rglob("*") if f.suffix in headerSuffixes]
    return (sorted(files))

def checkCommentPrefixes(s, issues):
    for match in regexpCommentOneLine.finditer(s):
        comment = match.group(1)
        if not regexpCommentValidContentPrefix.match(comment):
            issues.append(lineNum(match, s) + "contains an unclassified oneline comment, please mark as TODO or NOTE")
            break
    for match in regexpCommentMultiline.finditer(s):
        comment = match.group(1)
        if not regexpCommentValidContentPrefix.match(comment):
            issues.append(lineNum(match, s) + "contains an unclassified multiline comment, please mark as TODO or NOTE")
            break

def textChecks(f):
    s = f.read_text()
    issues = []
    for match in regexpUsing.finditer(s):
        issues.append(lineNum(match, s) + "contains 'using' directive (forbidden in headers)")
    checkCommentPrefixes(s, issues)
    return issues

def main():
    print("Running header-checker...")
    files = collectFiles(roots)
    if not files:
        print("No header files found.")
        return

    failed = False
    for f in files:
        issues = textChecks(f)
        if issues:
            failed = True
            print(f"{f}:\n" + "\n\t".join(issues))
    if failed:
        sys.exit(1)
    else:
        print("header-checker found no issues")
        print("WARNING!")
        print("This is a limited header-checker with only whatever checks are available in campus")
        print("A full header-checker can be more picky")

if __name__ == "__main__":
    main()
