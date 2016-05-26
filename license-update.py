#!/usr/bin/env python3

# If there is already a comment block, a year range through the current year is
# created using the first year in the comment. If there is no comment block, a
# new one is added containing just the current year.

from datetime import date
import os
import re
import sys

sep = os.sep
# If directory separator is backslash, escape it for regexes
if sep == "\\":
    sep += "\\"

# Files and directories which should be included in or excluded from the update
regexInclude = re.compile("\.cpp$|\.h$|\.hpp$|\.inc$|\.java$")
folderExclude = "build" + sep + "|\.git" + sep + "|gradle" + sep + \
                "|\.gradle" + sep + "|ni-libraries" + sep + "|ctre" + sep + \
                "|frccansae" + sep + "|FRC_FPGA_ChipObject" + sep + \
                "|gtest" + sep + "|msgs" + sep + "|i2clib" + sep + \
                "|NetworkCommunication" + sep + "|ni" + sep + \
                "|spilib" + sep + "|visa" + sep
regexExclude = re.compile(folderExclude +
                          "|NIIMAQdx\.h$|nivision\.h$|NIVisionJNI\.cpp$|"
                          "can_proto\.h$|jni\.h$|jni_md\.h$")

currentYear = str(date.today().year)

# Recursively create list of files in given directory
files = [os.path.join(dp, f) for dp, dn, fn in
         os.walk(os.path.expanduser(".")) for f in fn]

# Apply regex filters to list
files = [f for f in files if regexInclude.search(f)]
files = [f for f in files if not regexExclude.search(f)]

if not files:
    print("Error: no files to format", file=sys.stderr)
    sys.exit(1)

for name in files:
    # List names of files as they are processed if verbose flag was given
    if len(sys.argv) > 1 and sys.argv[1] == "-v":
        print("Processing", name,)
    with open(name, "r", encoding = "ISO-8859-1") as file:
        modifyCopyright = False
        year = ""

        # Get first line of file
        line = file.readline()

        # If first line is copyright comment
        if line[0:2] == "/*":
            modifyCopyright = True

            # Get next line
            line = file.readline()

            # Search for start of copyright year
            pos = line.find("20")

            # Extract it if found
            if pos != -1:
                year = line[pos:pos + 4]
            else:
                continue

            # Retrieve lines until one past end of comment block
            inComment = True
            inBlock = True
            while inBlock:
                if not inComment:
                    pos = line.find("/*", pos)
                    if pos != -1:
                        inComment = True
                    else:
                        inBlock = False
                else:
                    pos = line.find("*/", pos)
                    if pos != -1:
                        inComment = False

                    # This assumes no comments are started on the same line after
                    # another ends
                    line = file.readline()
                    pos = 0

        with open(name + ".tmp", "w", encoding = "ISO-8859-1") as temp:
            # Write first line of comment
            temp.write("/*")
            for i in range(0, 76):
                temp.write("-")
            temp.write("*/\n")

            # Write second line of comment
            temp.write("/* Copyright (c) FIRST ")
            if modifyCopyright and year != currentYear:
                temp.write(year)
                temp.write("-")
            temp.write(currentYear)
            temp.write(". All Rights Reserved.")
            for i in range(0, 24):
                temp.write(" ")
            if not modifyCopyright or year == currentYear:
                for i in range(0, 5):
                    temp.write(" ")
            temp.write("*/\n")

            # Write rest of lines of comment
            temp.write("/* Open Source Software - may be modified and shared by FRC teams. The code   */\n"
                       "/* must be accompanied by the FIRST BSD license file in the root directory of */\n"
                       "/* the project.                                                               */\n"
                       "/*")
            for i in range(0, 76):
                temp.write("-")
            temp.write("*/\n")

            # If line after comment block isn't empty
            if len(line) > 1 and line[0] != " ":
                temp.write("\n")
            temp.write(line)

            # Copy rest of original file into new one
            for line in file:
                temp.write(line)

    # Replace old file
    os.remove(name)
    os.rename(name + ".tmp", name)

