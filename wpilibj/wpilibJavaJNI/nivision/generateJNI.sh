#!/bin/bash
#This script should be able to generate the JNI
#   bindings for NIVision.  At some point,
#   it should be integrated into the build system.
# Assumes running from allwpilib/wpilibj/wpilibJavaJNI/nivision

# Get structure sizes.
python gen_struct_sizer.py ../../../wpilibc/wpilibC++Devices/include/nivision.h nivision_2011.ini
arm-frc-linux-gnueabi-gcc -I../../../wpilibc/wpilibC++Devices/include -S struct_sizer.c
cat struct_sizer.s | python get_struct_size.py > nivision_arm.ini

python gen_struct_sizer.py ../../../wpilibc/wpilibC++Devices/include/NIIMAQdx.h imaqdx.ini
arm-frc-linux-gnueabi-gcc -I../../../wpilibc/wpilibC++Devices/include -S struct_sizer.c
cat struct_sizer.s | python get_struct_size.py > imaqdx_arm.ini

# Run python generator.
python gen_java.py \
    ../../../wpilibc/wpilibC++Devices/include/nivision.h \
    nivision_arm.ini \
    nivision_2011.ini \
    \
    ../../../wpilibc/wpilibC++Devices/include/NIIMAQdx.h \
    imaqdx_arm.ini \
    imaqdx.ini \
    dxattr.h \
    imaqdx_arm.ini \
    dxattr.ini

# Stick generated files into appropriate places.
cp NIVision.cpp ../lib/NIVisionJNI.cpp
mkdir -p ../../wpilibJavaDevices/src/main/java/com/ni/vision
cp *.java ../../wpilibJavaDevices/src/main/java/com/ni/vision/
