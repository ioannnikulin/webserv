import sys

try:
    from clang.cindex import Index
    import sys

    try:
        Index.create()
    except:
        sys.exit(1)
except ImportError:
    sys.exit(1)

sys.exit(0)