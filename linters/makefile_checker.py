#!/usr/bin/env python3

import argparse
import pathlib
import re
import sys

exit_code = 0

def cppPresence(root, makefileText):
    global exit_code

    all_cpp = {
        str(p.name) # could have been relative path with folders, but we don't write them in makefile in that form anyway
        # so no filename collisions checked
        for p in root.rglob("*.cpp")
        if p.is_file()
    }
    regexpCommentedOutCpp = re.compile(r'#.*\.cpp')
    if regexpCommentedOutCpp.search(makefileText):
        print("Please do not comment out cpp files in the makefile")
        exit_code = 1

    make_cpp = set(re.findall(r"[A-Za-z0-9_.-]+\.cpp", makefileText))
    ignored_cpp = set(["cxx_runner.cpp"])

    missing_in_makefile = all_cpp - make_cpp - ignored_cpp
    extra_in_makefile = make_cpp - all_cpp - ignored_cpp

    if missing_in_makefile:
        print("These .cpp files exist on disk but are missing in the Makefile:")
        for f in sorted(missing_in_makefile):
            print("\t", f)
        exit_code = 1

    if extra_in_makefile:
        print("These .cpp files are listed in the Makefile but do not exist on disk:")
        for f in sorted(extra_in_makefile):
            print("\t", f)
    #    exit_code = 1 # not considered a failure for now

def main():
    global exit_code

    parser = argparse.ArgumentParser(
        description="Check that all .cpp files in a directory are listed in a Makefile (and vice versa)."
    )
    parser.add_argument(
        "directory",
        type=str,
        help="Root directory to search for .cpp files (recursively)."
    )
    parser.add_argument(
        "makefile",
        type=str,
        help="Path to the Makefile to check."
    )

    args = parser.parse_args()

    root = pathlib.Path(args.directory).resolve()
    makefile_path = pathlib.Path(args.makefile).resolve()

    if not root.exists() or not root.is_dir():
        print(f"Directory does not exist or is not a directory: {root}")
        sys.exit(1)

    if not makefile_path.exists():
        print(f"Makefile does not exist: {makefile_path}")
        sys.exit(1)

    text = makefile_path.read_text()
    cppPresence(root, text)

    if exit_code == 0:
        print("makefile-checker found no issues")

    sys.exit(exit_code)


if __name__ == "__main__":
    main()
