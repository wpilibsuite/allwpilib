# This task ensures that all source files have exactly one EOF newline.

import os
from task import Task

class Newline(Task):
    def run(self, name):
        newlines = 0

        # Remove all but one EOF newline, or append one if necessary
        eol = os.linesep
        if name.endswith("bat"):
            eol = "\r\n"
        with open(name, "r+", newline = eol) as file:
            # Get file size
            file.seek(0, os.SEEK_END)
            size = file.tell()

            # Seek to last character in file
            file.seek(size - 1)

            # While last character is a newline
            while file.read(1) == "\n":
                newlines = newlines + 1

                # Seek to character before newline
                file.seek(size - 1 - len(eol) * newlines)

            if newlines < 1:
                # Append newline to end of file
                file.seek(size)
                file.write("\n")
            elif newlines > 1:
                # Truncate all but one newline
                file.truncate(size - len(eol) * (newlines - 1))
