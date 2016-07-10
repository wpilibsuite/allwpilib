# Provides a task base class for use by format.py

from abc import *
import os
import re

sep = os.sep
# If directory separator is backslash, escape it for regexes
if sep == "\\":
    sep += "\\"

# List of folder regexes which should be excluded from modification
folderExclude = \
    [name + sep for name in [
     "\.git",
     "\.gradle",
     "FRC_FPGA_ChipObject",
     "NetworkCommunication",
     "__pycache__",
     "build",
     "ctre",
     "frccansae",
     "gtest",
     "i2clib",
     "msgs",
     "ni-libraries",
     "ni" + sep + "vision",
     "spilib",
     "wpilibj" + sep + "src" + sep + "athena" + sep + "cpp" + sep + "include",
     "wpilibj" + sep + "src" + sep + "athena" + sep + "cpp" + sep + "lib",
     "wpilibj" + sep + "src" + sep + "athena" + sep + "cpp" + sep + "nivision",
     "visa"]]

# List of file regexes which should be excluded from modification
fileExclude = [name + "$" for name in [
               "CanTalonSRX\.h",
               "NIIMAQdx\.h",
               "can_proto\.h",
               "nivision\.h",
               "\.jar",
               "\.patch",
               "\.png",
               "\.py",
               "\.so"]]

# Regex of exclusions
regexExclude = re.compile("|".join(folderExclude + fileExclude))

class Task(object):
    __metaclass__ = ABCMeta

    def __init__(self):
        self.regexInclude = re.compile("|".join(["\." + ext + "$" for ext in
                                                 self.getIncludeExtensions()]))

    # Extensions of files which should be included in processing
    def getIncludeExtensions(self):
        return []

    # Perform task on file with given name
    @abstractmethod
    def run(self, name):
        return

    # Returns True if file isn't generated (generated files are skipped)
    @staticmethod
    def notGeneratedFile(name):
        return not regexExclude.search(name)

    # Returns True if file has an extension this task can process
    def fileMatchesExtension(self, name):
        if self.getIncludeExtensions() != []:
            return self.regexInclude.search(name)
        else:
            return True
