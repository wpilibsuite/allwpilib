#!/bin/bash
#This script should be able to generate the JNI
#   bindings for NIVision.  At some point,
#   it should be integrated into the build system.
# Assumes running from allwpilib/wpilibj/wpilibJavaJNI/nivision
# Get files that we node to generate from.
# Run python generator.
python gen_java.py ../../../wpilibc/wpilibC++Devices/include/nivision.h nivision_arm.ini nivision_2011.ini
# Stick generated files into appropriate places.
cp NIVision.cpp ../lib/NIVisionJNI.cpp
mkdir -p ../../wpilibJavaDevices/src/main/java/com/ni/vision
cp *.java ../../wpilibJavaDevices/src/main/java/com/ni/vision/
