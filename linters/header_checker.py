#!/usr/bin/env python3

import sys
import re
from pathlib import Path
from clang import cindex

roots = sys.argv[1:] if len(sys.argv) > 1 else ["sources", "tests"]
headerSuffixes = {".h", ".hpp", ".hh", ".hxx"}

regexpUsing = re.compile(r"^\s*using\s", re.M)
regexpCommentOneLine = re.compile(r'(?<!http:)(?<!https:)//(.*)')
regexpCommentMultiline = re.compile(r'/\*(.*?)\*/', re.DOTALL)
regexpCommentValidContentPrefix = re.compile(r'^(( TODO [0-9]+:)|( NOTE:)|( namespace)|( clang-format)|( NOLINT))')
regexpIncludeRelative = re.compile(r'#include ".*\.\.')
regexpIfndef = re.compile(r"^\s*#\s*ifndef\s+([A-Za-z_][A-Za-z0-9_]*)", re.M)
regexpDefine = lambda name: re.compile(r"^\s*#\s*define\s+" + re.escape(name) + r"\b", re.M)
regexpEndif = re.compile(r"^\s*#\s*endif\b", re.M)

def lineNum(match, s):
    start_pos = match.start()
    return str(s.count('\n', 0, start_pos + 1) + 1) + ":"

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
            issues.append(f'{f}: guard name "{guardName}" does not match expected "{expectedGuard}"')
    
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

def safe_kind(node):
    try:
        return node.kind
    except ValueError:
        return None

def getNamespaces(node, ns_stack=None):
    if ns_stack is None:
        ns_stack = [] # we're currently at level zero of possible namespace enclosures
    result = []
    kind = safe_kind(node)
    if kind == cindex.CursorKind.NAMESPACE:
        ns_stack = ns_stack + [node.spelling] # if we're on a namespace declaration, increase the level

    if kind in (cindex.CursorKind.FUNCTION_DECL,
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
        safe_kind(node) == cindex.CursorKind.FUNCTION_DECL and
        node.spelling == "main"
    )

def checkAST(file, index, roots):
    args=['-std=c++98'] + [f'-I{p}' for p in roots if Path(p).is_dir()] + ['-I/usr/include', '-I/usr/local/include']
    tu = index.parse(str(file), args=args)
    tu_path = Path(tu.spelling)

    errors = [d for d in tu.diagnostics if d.severity >= d.Error]
    if errors:
        for d in errors:
            print(d)
        return["AST parse failed; fix includes first"]

    issues = []

    for node, ns_stack in getNamespaces(tu.cursor):
        if not node.location.file or Path(node.location.file.name) != tu_path:
            continue

        if not ns_stack and not isMain(node):
            issues.append(f"{file}: {node.spelling} is not inside any namespace\n")

        if safe_kind(node) == cindex.CursorKind.CLASS_DECL:
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
    index = cindex.Index.create()
    files = collectFiles(roots)
    if not files:
        print("No header files found.")
        return

    failed = False
    for f in files:
        issues = textChecks(f)
        issues += checkAST(f, index, roots)
        if issues:
            failed = True
            print("\n".join(issues))
    if failed:
        sys.exit(1)
    else:
        print("header-checker: no issues")
        print("-------------------------------------------------")

if __name__ == "__main__":
    main()
