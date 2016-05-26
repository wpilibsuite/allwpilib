# This task runs clang-format on all C++ source files.

import subprocess
import sys
from task import Task

class ClangFormat(Task):
    def getIncludeExtensions(self):
        return ["cpp", "h", "inc"]

    def run(self, name):
        # Run clang-format
        if subprocess.call(["clang-format", "-i", "-style=file", name]) == -1:
            print("Error: clang-format not found in PATH. Is it installed?",
                  file = sys.stderr)
