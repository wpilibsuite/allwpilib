# This task removes trailing whitespace from all source files.

import os
from task import Task

class Whitespace(Task):
    def run(self, name):
        # Remove trailing whitespace
        fileChanged = False
        with open(name, "r") as file:
            with open(name + ".tmp", "w") as temp:
                for line in file:
                    processedLine = line[0:len(line) - 1].rstrip() + "\n"
                    if not fileChanged and len(line) != len(processedLine):
                        fileChanged = True
                    temp.write(processedLine)

        # Replace old file if it was changed
        if fileChanged:
            os.remove(name)
            os.rename(name + ".tmp", name)
        else:
            os.remove(name + ".tmp")
