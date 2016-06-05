#!/usr/bin/env python3

import os

sep = os.sep
# If directory separator is backslash, escape it for regexes
if sep == "\\":
    sep += "\\"

# List of folders which should be excluded from modification
def folderExclude():
    return [name + sep for name in [
            "\.git",
            "\.gradle",
            "FRC_FPGA_ChipObject",
            "NetworkCommunication",
            "build",
            "ctre",
            "frccansae",
            "gtest",
            "i2clib",
            "msgs",
            "ni-libraries",
            "ni" + sep + "vision",
            "__pycache__",
            "spilib",
            "visa"]]

# List of files which should be excluded from modification
def fileExclude():
    return [name + "$" for name in [
            "CanTalonSRX\.h",
            "NIIMAQdx\.h",
            "nivision\.h",
            "can_proto\.h",
            "CanTalonJNI\.cpp",
            "NIVisionJNI\.cpp",
            "JNI\.cpp",
            "jni\.h",
            "jni_md\.h",
            "\.jar",
            "\.png",
            "\.py",
            "\.so"]]

# Regex of exclusions
def regexExclude():
    return "|".join(folderExclude() + fileExclude())
