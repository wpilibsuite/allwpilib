#!/usr/bin/env python3

# This script generates the network communication interface for wpilibj.
#
# This script takes no arguments and should be invoked from either the gen
# directory or the root directory of the project.

from datetime import date
import os
import re
import subprocess

# Check that the current directory is part of a Git repository
def inGitRepo(directory):
    ret = subprocess.run(["git", "rev-parse"], stderr = subprocess.DEVNULL)
    return ret.returncode == 0

def main():
    if not inGitRepo("."):
        print("Error: not invoked within a Git repository", file = sys.stderr)
        sys.exit(1)

    # Handle running in either the root or gen directories
    configPath = "."
    if os.getcwd().rpartition(os.sep)[2] == "gen":
        configPath = ".."

    outputName = configPath + \
        "/wpilibj/src/athena/java/edu/wpi/first/wpilibj/hal/FRCNetComm.java"

    # Set initial copyright year and get current year
    year = "2016"
    currentYear = str(date.today().year)

    # Start writing output file
    with open(outputName + ".tmp", "w") as temp:
        # Write first line of comment
        temp.write("/*")
        for i in range(0, 76):
            temp.write("-")
        temp.write("*/\n")

        # Write second line of comment
        temp.write("/* Copyright (c) FIRST ")
        if year != currentYear:
            temp.write(year)
            temp.write("-")
        temp.write(currentYear)
        temp.write(". All Rights Reserved.")
        for i in range(0, 24):
            temp.write(" ")
        if year == currentYear:
            for i in range(0, 5):
                temp.write(" ")
        temp.write("*/\n")

        # Write rest of lines of comment
        temp.write("""\
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*""")
        for i in range(0, 76):
            temp.write("-")
        temp.write("*/\n")

        # Write preamble
        temp.write("""
package edu.wpi.first.wpilibj.hal;

import edu.wpi.first.wpilibj.hal.JNIWrapper;

/**
 * JNI wrapper for library <b>FRC_NetworkCommunication</b><br>.
 */
@SuppressWarnings(\"MethodName\")
public class FRCNetComm extends JNIWrapper {
""")

        # Read enums from C++ source files
        firstEnum = True
        for fileName in ["LoadOut.h", "UsageReporting.h"]:
            with open(configPath + "/hal/include/FRC_NetworkCommunication/" + \
                      fileName, "r") as cppSource:
                while True:
                    # Read until an enum is encountered
                    line = ""
                    pos = -1
                    while line.find("enum") == -1:
                        line = cppSource.readline()
                        if line == "":
                            break

                    if line == "":
                        break

                    # If "{" is on next line, read next line
                    if line.find("{") == -1:
                        line = cppSource.readline()

                    # Write enum to output file as interface
                    values = []
                    line = cppSource.readline()
                    while line.find("}") == -1:
                        if line == "\n":
                            values.append("")
                        else:
                            if line[0] != "#":
                                line = line.strip()
                                if line[len(line) - 1] == ",":
                                    values.append(line[0:len(line) - 1])
                                else:
                                    values.append(line)
                        line = cppSource.readline()

                    # Extract enum name
                    nameStart = 0
                    for i, c in enumerate(line):
                        if c != " " and c != "}":
                            nameStart = i
                            break
                    enumName = line[nameStart:len(line) - 2]

                    # Write comment for interface name
                    # Only add newline if not the first enum
                    if firstEnum == True:
                        firstEnum = False
                    else:
                        temp.write("\n")
                    temp.write("  /**\n"
                               "   * ")

                    # Splits camelcase string into words
                    enumCamel = re.findall(r'[A-Z](?:[a-z]+|[A-Z]*(?=[A-Z]|$))',
                                           enumName)
                    temp.write(enumCamel[0] + " ")
                    for i in range(1, len(enumCamel)):
                        temp.write(enumCamel[i][0].lower() + \
                            enumCamel[i][1:len(enumCamel[i])] + " ")
                    temp.write("from " + fileName + "\n"
                               "   */\n"
                               "  @SuppressWarnings(\"TypeName\")\n"
                               "  public interface " + enumName + " {\n")

                    # Write enum values
                    count = 0
                    for value in values:
                        # Pass through empty lines
                        if value == "":
                            temp.write("\n")
                            continue

                        if value.find("=") == -1:
                            value = value + " = " + str(count)
                            count += 1

                        # Add scoping for values from a different enum
                        if enumName != "tModuleType" and \
                                value.find("kModuleType") != -1:
                            value = value.replace("kModuleType",
                                                  "tModuleType.kModuleType")
                        temp.write("    int " + value[0:len(value)] + ";\n")

                    # Write end of enum
                    temp.write("  }\n")

        # Write closing brace for file
        temp.write("}\n")

    # Replace old output file
    try:
        os.remove(outputName)
    except OSError:
        pass
    os.rename(outputName + ".tmp", outputName)

if __name__ == "__main__":
    main()
