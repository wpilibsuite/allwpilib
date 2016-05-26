#!/usr/bin/env python3

import os
import re
import sys

sep = os.sep
# If directory separator is backslash, escape it for regexes
if sep == "\\":
    sep += "\\"

# Files and directories which should be included in or excluded from the update
folderExclude = "build" + sep + "|\.git" + sep + "|\.gradle" + sep + \
                "|ni-libraries" + sep + "|ctre" + sep + "|frccansae" + sep + \
                "|FRC_FPGA_ChipObject" + sep + "|gtest" + sep + \
                "|i2clib" + sep + "|NetworkCommunication" + sep + \
                "|spilib" + sep + "|visa" + sep
regexExclude = re.compile(folderExclude +
                          "|NIIMAQdx\.h$|nivision\.h$|can_proto\.h$|"
                          "CanTalonSRX\.h$|\.jar$|\.png$|\.so")

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
files = [f for f in files if not regexExclude.search(f)]

if not files:
    print("Error: no files found to format", file=sys.stderr)
    sys.exit(1)

for name in files:
    # List names of files as they are processed if verbose flag was given
    if len(sys.argv) > 1 and sys.argv[1] == "-v":
        print("Processing", name,)

    newlines = 0

    # Remove all EOF newlines, then append one
    with open(name, "r+", encoding = "ISO-8859-1") as file:
        file.seek(0, os.SEEK_END)
        size = file.tell()
        file.seek(size - 1)
        while file.read(1) == os.linesep:
            newlines = newlines + 1
            size = size - 1
            file.seek(size - 1)

        if newlines < 1:
            file.seek(size)
            file.write(os.linesep)
        elif newlines > 1:
            file.seek(size)
            file.truncate()
