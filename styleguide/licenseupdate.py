# This task updates the license comment block at the top of all source files.

# If there is already a comment block, a year range through the current year is
# created using the first year in the comment. If there is no comment block, a
# new one is added containing just the current year.

from datetime import date
from functools import partial
import hashlib
import os
from task import Task

currentYear = str(date.today().year)

class LicenseUpdate(Task):
    def getIncludeExtensions(self):
        return ["cpp", "h", "inc", "java"]

    def run(self, name):
        with open(name, "r") as file:
            modifyCopyright = False
            year = ""

            # Get first line of file
            line = file.readline()

            # If first line is non-documentation comment
            if line[0:3] == "/*\n" or line[0:3] == "/*-":
                modifyCopyright = True

                # Get next line
                line = file.readline()

                # Search for start of copyright year
                pos = line.find("20")

                # Extract it if found. If not, rewrite whole comment
                if pos != -1:
                    year = line[pos:pos + 4]
                else:
                    modifyCopyright = False

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

                        # This assumes no comments are started on the same line
                        # after another ends
                        line = file.readline()
                        pos = 0

            with open(name + ".tmp", "w") as temp:
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
                temp.write("""\
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*""")
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

        # Replace old file if it was changed
        if self.md5sum(name) != self.md5sum(name + ".tmp"):
            os.remove(name)
            os.rename(name + ".tmp", name)
        else:
            os.remove(name + ".tmp")

    # Compute MD5 sum of file
    @staticmethod
    def md5sum(name):
        with open(name, mode = "rb") as file:
            digest = hashlib.md5()
            for buf in iter(partial(file.read, 128), b""):
                digest.update(buf)
        return digest.hexdigest()
