# This task runs cpplint.py on all C++ source files.

"""
cpplint.py was converted from python 2 to python 3 with the following command
and a patch was applied:

2to3 -f all -f buffer -f idioms -f set_literal -f ws_comma -nw cpplint.py

diff --git a/styleguide/cpplint.py b/styleguide/cpplint.py
--- a/styleguide/cpplint.py
+++ b/styleguide/cpplint.py
@@ -6094,13 +6094,6 @@ def ParseArguments(args):
 def main():
   filenames = ParseArguments(sys.argv[1:])
 
-  # Change stderr to write with replacement characters so we don't die
-  # if we try to print something containing non-ASCII characters.
-  sys.stderr = codecs.StreamReaderWriter(sys.stderr,
-                                         codecs.getreader('utf8'),
-                                         codecs.getwriter('utf8'),
-                                         'replace')
-
   _cpplint_state.ResetErrorCounts()
   for filename in filenames:
     ProcessFile(filename, _cpplint_state.verbose_level)
"""

import os
import subprocess
import sys
from task import Task

class Lint(Task):
    def getIncludeExtensions(self):
        return ["cpp", "h", "inc"]

    def run(self, name):
        # Handle running in either the root or styleguide directories
        cpplintPrefix = ""
        if os.getcwd().rpartition(os.sep)[2] != "styleguide":
            cpplintPrefix = "styleguide/"

        # Run cpplint.py
        proc = subprocess.Popen(["python", cpplintPrefix + "cpplint.py",
                                 "--filter="
                                 "-build/c++11,"
                                 "-build/header_guard,"
                                 "-build/include,"
                                 "-build/namespaces,"
                                 "-readability/todo,"
                                 "-runtime/references,"
                                 "-runtime/string",
                                 "--extensions=cpp,h,inc",
                                 name], bufsize = 1, stderr = subprocess.PIPE)
        for line in proc.stderr:
            if b"Done processing" not in line and b"Total errors" not in line:
                print(line.rstrip().decode(sys.stdout.encoding))
