from clang.cindex import Index
import sys

try:
    Index.create()
except:
    sys.exit(1)