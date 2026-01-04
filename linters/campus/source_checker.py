#!/usr/bin/env python3

import sys
import re
from pathlib import Path
from header_checker import checkCommentPrefixes
from header_checker import lineNum
from header_checker import regexpIncludeRelative

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
regexpThrow = re.compile(r"^\s*throw\([^)]", re.M)
regexpReturn = re.compile(r"^\s*return(.*?);", re.M | re.S)
regexpValidReturnContent = re.compile(
    r"""
^
(
    \s*                 # return;
  |
    \s+\(.*\)           # return ( ... )
)
$
""",
    re.S | re.X
)
regexpPoll = re.compile(f"^bpoll\\(")
regexpErrno = re.compile(r".*\n.*\n.*\n.*\berrno\b")
regexpIoOperations = re.compile(r"\b(read|write|recv|send)\(")

def main():
    print("Running source-checker...")
    files = collectFiles(roots)
    if not files:
        print("No source files found.")
        sys.exit(0)

    failed = False
    pollCalls = 0

    for f in files:
        s = f.read_text()

        issues = []

        for match in regexpReturn.finditer(s):
            if not regexpValidReturnContent.match(match.group(1)):
                issues.append(f"{f}:{lineNum(match, s)} please wrap returned values in parenthesis, preceded by a space; for void use `return;`")

        for match in regexpThrow.finditer(s):
            issues.append(f"{f}:{lineNum(match, s)} please do not wrap thrown values in parenthesis; for no value in declarations use `throw();`")

        for match in regexpUsingNamespace.finditer(s):
            issues.append(f"{f}:{lineNum(match, s)} contains 'using namespace' directive; please switch to explicit directive like 'using std::string' etc.")

        checkCommentPrefixes(s, issues)

        for match in regexpIncludeRelative.finditer(s):
            issues.append(f"{f}:{lineNum(match, s)} please specify the path starting from the project source root")

        for match in regexpErrno.finditer(s):
            if regexpIoOperations.search(match.group(0)):
                issues.append(f"{f}:{lineNum(match, s)} using errno with IO operations is forbidden by the subject")
            
        if regexpPoll.search(s):
            pollCalls += 1

        if issues:
            failed = True
            print("\n".join(issues))

    if pollCalls > 1:
        failed = True
        print("too many poll() calls detected")

    if failed:
        sys.exit(1)
    else:
        print("source checker didn't find any issues")
        print("WARNING!")
        print("This is a limited source-checker with only whatever checks are available in campus")
        print("A full source-checker can be more picky")
        print("-------------------------------------------------")

if __name__ == "__main__":
    main()