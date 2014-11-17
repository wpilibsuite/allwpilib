#!/bin/bash
#This script should be able to generate the JNI
#   bindings for the CANTalon using swig.At some point,
#   it should be integrated into the build system,
#   but I[james 18 November 2014] don't know how to do that.
# Assumes running from allwpilib/wpilibj/wpilibJavaJNI/swigTalon
# Get files that we node to generate from.
cp ../../../hal/lib/Athena/ctre/CanTalonSRX.cpp ./
cp ../../../wpilibc/wpilibC++Devices/include/ctre/* ./
# Clean up from previous run.
rm *.java
# Run SWIG.
swig -c++ -package edu.wpi.first.wpilibj.hal -java CanTalonSRX.i
# Stick generated files into appropriate places.
cp CanTalonSRX_wrap.cxx ../lib/CanTalonSRXJNI.cpp
mv CanTalonJNI.java ../../wpilibJavaDevices/src/main/java/edu/wpi/first/wpilibj/hal/
cp *.java ../../wpilibJavaDevices/src/main/java/edu/wpi/first/wpilibj/
