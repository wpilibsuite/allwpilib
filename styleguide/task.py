# Provides a task base class for use by format.py

from abc import *
import os
import re

sep = os.sep
# If directory separator is backslash, escape it for regexes
if sep == "\\":
    sep += "\\"

# There are two groups of regexes which prevent tasks from running on matching
# files:
#   1) generated files (shouldn't be modified)
#   2) modifiable files
#
# format.py excludes matches for the "modifiable" regex before checking for
# modifications to generated files because some of the regexes from each group
# overlap.

# List of regexes for folders which contain generated files
genFolderExclude = \
    [name + sep for name in [
     "FRC_FPGA_ChipObject",
     "NetworkCommunication",
     "ctre",
     "frccansae",
     "gtest",
     "i2clib",
     "msgs",
     "ni-libraries",
     "ni" + sep + "vision",
     "spilib",
     "wpilibj" + sep + "src" + sep + "athena" + sep + "cpp" + sep + "nivision",
     "visa"]]

# List of regexes for generated files
genFileExclude = [name + "$" for name in [
                  "CanTalonSRX\.h",
                  "NIIMAQdx\.h",
                  "can_proto\.h",
                  "nivision\.h"]]

# Regex for generated file exclusions
genRegexExclude = re.compile("|".join(genFolderExclude + genFileExclude))

# Regex for folders which contain modifiable files
modifiableFolderExclude = \
    [name + sep for name in [
     "\.git",
     "\.gradle",
     "__pycache__",
     "build",
     "wpilibj" + sep + "src" + sep + "athena" + sep + "cpp" + sep + "include",
     "wpilibj" + sep + "src" + sep + "athena" + sep + "cpp" + sep + "lib"]]

# List of regexes for modifiable files
modifiableFileExclude = [name + "$" for name in [
                         "\.jar",
                         "\.patch",
                         "\.png",
                         "\.py",
                         "\.so"]]

# Regex for modifiable file exclusions
modifiableRegexExclude = \
    re.compile("|".join(modifiableFolderExclude + modifiableFileExclude))

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

    # Returns True if file is modifiable but should not have tasks run on it
    @staticmethod
    def isModifiableFile(name):
        return modifiableRegexExclude.search(name)

    # Returns True if file isn't generated (generated files are skipped)
    @staticmethod
    def isGeneratedFile(name):
        return genRegexExclude.search(name)

    # Returns True if file has an extension this task can process
    def fileMatchesExtension(self, name):
        if self.getIncludeExtensions() != []:
            return self.regexInclude.search(name)
        else:
            return True
