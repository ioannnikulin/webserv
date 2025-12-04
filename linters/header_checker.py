#!/usr/bin/env python3

import sys
import re
from pathlib import Path
from clang import cindex

roots = sys.argv[1:] if len(sys.argv) > 1 else ["sources", "include", "tests"]
headerSuffixes = {".h", ".hpp", ".hh", ".hxx"}

regexpUsing = re.compile(r"^\s*using\s", re.M)
regexpCommentOneLine = re.compile(r'//(.*)')
regexpCommentMultiline = re.compile(r'/\*(.*?)\*/', re.DOTALL)
regexpCommentValidContentPrefix = re.compile(r'^(( TODO [0-9]+:)|( NOTE:)|( namespace)|( clang-format)|( NOLINT))')
regexpIncludeRelative = re.compile(r'#include ".*\\.')

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
    for match in regexpIncludeRelative.finditer(s):
        issues.append(lineNum(match, s) + "please specify the path starting from the project source root")
    for match in regexpUsing.finditer(s):
        issues.append(lineNum(match, s) + "contains 'using' directive (forbidden in headers)")
    checkCommentPrefixes(s, issues)
    return issues

def getNamespaces(node, ns_stack=None):
    if ns_stack is None:
        ns_stack = [] # we're currently at level zero of possible namespace enclosures
    result = []

    if node.kind == cindex.CursorKind.NAMESPACE:
        ns_stack = ns_stack + [node.spelling] # if we're on a namespace declaration, increase the level

    if node.kind in (cindex.CursorKind.FUNCTION_DECL,
                     cindex.CursorKind.VAR_DECL,
                     cindex.CursorKind.CLASS_DECL,
                     cindex.CursorKind.STRUCT_DECL):
        result.append((node, ns_stack))

    # recursive call, e.g. we're in a namespace already, let's study its children
    for c in node.get_children():
        result += getNamespaces(c, ns_stack)
    return result

def isMain(node):
    return (
        node.kind == cindex.CursorKind.FUNCTION_DECL and
        node.spelling == "main"
    )

def checkAST(file):
    index = cindex.Index.create()
    tu = index.parse(str(file), args=['-std=c++98'])
    tu_path = Path(tu.spelling)
    issues = []

    for node, ns_stack in getNamespaces(tu.cursor):
        if Path(node.location.file.name) != tu_path:
            continue

        if not ns_stack and not isMain(node):
            issues.append(f"{file}: {node.spelling} is not inside any namespace\n")

        if node.kind == cindex.CursorKind.CLASS_DECL:
            cls_name = node.spelling
            has_ctor = False
            has_dtor = False
            has_copy_ctor = False
            has_copy_assign = False
            for c in node.get_children():
                if c.kind == cindex.CursorKind.CONSTRUCTOR:
                    has_ctor = True
                    # copy ctor
                    if any(p.type.spelling.startswith(("const " + "::".join(ns_stack) + "::" + cls_name + " &", "const " + cls_name + " &")) for p in c.get_arguments()):
                        # yes, that space before the ampersand is intentional, even though there is no space in our actual code
                        has_copy_ctor = True
                elif c.kind == cindex.CursorKind.DESTRUCTOR:
                    has_dtor = True
                elif c.kind == cindex.CursorKind.CXX_METHOD and c.spelling == 'operator=':
                    has_copy_assign = True
            missing = []
            if not has_ctor: missing.append("constructor")
            if not has_dtor: missing.append("destructor")
            if not has_copy_ctor: missing.append("copy constructor")
            if not has_copy_assign: missing.append("copy assignment")
            if missing:
                issues.append(f"{file}:{cls_name} - missing {', '.join(missing)}")
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
        issues += checkAST(f)
        if issues:
            failed = True
            print(f"{f}: " + "\n\t".join(issues))
    if failed:
        sys.exit(1)
    else:
        print("header-checker found no issues")

if __name__ == "__main__":
    main()
