#!/usr/bin/env python3

import os

sep = os.sep
# If directory separator is backslash, escape it for regexes
if sep == "\\":
    sep += "\\"

# Files and directories which should be included in or excluded from the update
def folderExclude():
    return "\.git" + sep + "|\.gradle" + sep + "|FRC_FPGA_ChipObject" + sep + \
           "|NetworkCommunication" + sep + "|build" + sep + "|ctre" + sep + \
           "|frccansae" + sep + "|gtest" + sep + "|i2clib" + sep + \
           "|msgs" + sep + "|ni-libraries" + sep + \
           "|ni" + sep + "vision" + sep + \
           "|spilib" + sep + "|visa" + sep

def regexExclude():
    return folderExclude() + "|CanTalonSRX\.h$|NIIMAQdx\.h$|nivision\.h$" + \
           "|can_proto\.h$|CanTalonJNI\.cpp$|NIVisionJNI\.cpp$|JNI\.cpp$" + \
           "|jni\.h$|jni_md\.h$|\.jar$|\.png$|\.py$|\.so"
