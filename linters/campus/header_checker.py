#!/usr/bin/env python3

import sys
import re
from pathlib import Path

roots = sys.argv[1:] if len(sys.argv) > 1 else ["sources", "include", "tests"]
headerSuffixes = {".h", ".hpp", ".hh", ".hxx"}

regexpUsing = re.compile(r"^\s*using\s", re.M)
regexpCommentOneLine = re.compile(r'//(.*)')
regexpCommentMultiline = re.compile(r'/\*(.*?)\*/', re.DOTALL)
regexpCommentValidContentPrefix = re.compile(r'^(( TODO [0-9]+:)|( NOTE:)|( namespace)|( clang-format)|( NOLINT))')
regexpIncludeRelative = re.compile(r'#include ".*\.\.')
regexpIfndef = re.compile(r"^\s*#\s*ifndef\s+([A-Za-z_][A-Za-z0-9_]*)", re.M)
regexpDefine = lambda name: re.compile(r"^\s*#\s*define\s+" + re.escape(name) + r"\b", re.M)
regexpEndif = re.compile(r"^\s*#\s*endif\b", re.M)

def lineNum(match, s):
    start_pos = match.start()
    return str(s.count('\n', 0, start_pos + 1) + 1) + ": "

def collectFiles(roots):
    files = []
    for r in roots:
        p = Path(r)
        if p.is_file() and p.suffix in headerSuffixes:
            files.append(p)
        elif p.is_dir():
            files += [f for f in p.rglob("*") if f.suffix in headerSuffixes]
    return (sorted(files))

def screamingSnakeFromFilename(path: Path):
    # SCREAMING_SNAKE of the filename (basename including extension)
    # e.g. "WebServer.hpp" -> "WEBSERVER_HPP"
    name = path.name
    # replace non-alnum with underscore
    macro = re.sub(r"[^0-9A-Za-z]", "_", name).upper()
    # collapse consecutive underscores
    macro = re.sub(r"_+", "_", macro)
    # strip leading/trailing underscores
    macro = macro.strip("_")
    return macro

def checkCommentPrefixes(f, s, issues):
    for match in regexpCommentOneLine.finditer(s):
        comment = match.group(1)
        if not regexpCommentValidContentPrefix.match(comment):
            issues.append(f"{f}:{lineNum(match, s)} contains an unclassified oneline comment, please mark as TODO or NOTE")
    for match in regexpCommentMultiline.finditer(s):
        comment = match.group(1)
        if not regexpCommentValidContentPrefix.match(comment):
            issues.append(f"{f}:{lineNum(match, s)} contains an unclassified multiline comment, please mark as TODO or NOTE")

def checkGuard(f, s, issues):
    guardMatch = regexpIfndef.search(s)
    hasGuard = False
    guardName = None
    if guardMatch:
        guardName = guardMatch.group(1)
        if regexpDefine(guardName).search(s) and regexpEndif.search(s):
            hasGuard = True

    expectedGuard = screamingSnakeFromFilename(f)
    if not hasGuard:
        issues.append(f"{f}: missing/mismatched include guard")
    else:
        if guardName != expectedGuard:
            issues.append(f"{f}: guard name \"{guardName}\" does not match expected \"{expectedGuard}\"")

def textChecks(f):
    s = f.read_text()
    issues = []
    for match in regexpIncludeRelative.finditer(s):
        issues.append(f"{f}:{lineNum(match, s)} please specify the path starting from the project source root")
    for match in regexpUsing.finditer(s):
        issues.append(f"{f}:{lineNum(match, s)} contains 'using' directive (forbidden in headers)")
    checkCommentPrefixes(f, s, issues)
    checkGuard(f, s, issues)
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
            print("\n".join(issues))
    if failed:
        sys.exit(1)
    else:
        print("header-checker found no issues")
        print("WARNING!")
        print("This is a limited header-checker with only whatever checks are available in campus")
        print("A full header-checker can be more picky")
        print("-------------------------------------------------")

if __name__ == "__main__":
    main()
