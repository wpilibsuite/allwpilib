#!/usr/bin/env python3

# This script runs all formatting tasks on the code base.
#
# Passing "-v" as an argument enables verbosity. Otherwise, this script takes no
# arguments. This should be invoked from either the styleguide directory or the
# root directory of the project.

import os
import subprocess
import sys

from clangformat import ClangFormat
from licenseupdate import LicenseUpdate
from newline import Newline
from whitespace import Whitespace
from task import Task

# Check that the current directory is part of a Git repository
def inGitRepo(directory):
    ret = subprocess.run(["git", "rev-parse"], stderr = subprocess.DEVNULL)
    return ret.returncode == 0

def main():
    if not inGitRepo("."):
        print("Error: not invoked within a Git repository", file = sys.stderr)
        sys.exit(1)

    # Handle running in either the root or styleguide directories
    configPath = ""
    if os.getcwd().rpartition(os.sep)[2] == "styleguide":
        configPath = ".."
    else:
        configPath = "."

    # Recursively create list of files in given directory
    files = [os.path.join(dp, f) for dp, dn, fn in
             os.walk(os.path.expanduser(configPath)) for f in fn]

    if not files:
        print("Error: no files found to format", file = sys.stderr)
        sys.exit(1)

    # Don't format generated files
    files = [name for name in files if Task.notGeneratedFile(name)]

    clangFormat = ClangFormat()
    licenseUpdate = LicenseUpdate()
    newline = Newline()
    whitespace = Whitespace()

    # Check for verbose flag
    isVerbose = len(sys.argv) > 1 and sys.argv[1] == "-v"

    for name in files:
        if isVerbose:
            print("Processing", name,)

        for task in [clangFormat, licenseUpdate, newline, whitespace]:
            if task.fileMatchesExtension(name):
                if isVerbose:
                    print("  with " + type(task).__name__)
                task.run(name)

if __name__ == "__main__":
    main()
