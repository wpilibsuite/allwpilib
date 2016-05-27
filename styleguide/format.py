#!/usr/bin/env python3

from subprocess import call
import os
import re
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import wpi

# Files and directories which should be included in or excluded from processing
regexInclude = re.compile("\.cpp$|\.h$|\.hpp$|\.inc$")
regexExclude = re.compile(wpi.regexExclude())

# Handle running in either the root or styleguide directories
configPath = ""
if os.getcwd().rpartition(os.sep)[2] == "styleguide":
    configPath = ".."
else:
    configPath = "."

# Recursively create list of files in given directory
files = [os.path.join(dp, f) for dp, dn, fn in
         os.walk(os.path.expanduser(configPath)) for f in fn]

# Apply regex filters to list
files = [f for f in files if regexInclude.search(f)]
files = [f for f in files if not regexExclude.search(f)]

# Set clang-format name for platform
clangExec = "clang-format"
if sys.platform.startswith("win32"):
    clangExec += ".exe"

if not files:
    print("Error: no files found to format", file=sys.stderr)
    sys.exit(1)

for name in files:
    # List names of files as they are processed if verbose flag was given
    if len(sys.argv) > 1 and sys.argv[1] == "-v":
        print("Processing", name,)

    # Run clang-format
    call([clangExec, "-i", "-style=file", name])
