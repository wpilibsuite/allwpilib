#!/bin/sh
opencv_dir=`find /workspace/.gradle/caches -name libopencv_core.so.3.4 | grep x86-64 | xargs dirname`
env \
    HALSIM_EXTENSIONS=simulation/halsim_gui/build/libs/halsim_gui/shared/linuxx86-64/release/libhalsim_gui.so \
    LD_LIBRARY_PATH=cameraserver/build/libs/cameraserver/shared/linuxx86-64/release/:ntcore/build/libs/ntcore/shared/linuxx86-64/release/:ntcore/build/libs/ntcoreJNIShared/shared/linuxx86-64/release/:cscore/build/libs/cscore/shared/linuxx86-64/release/:cscore/build/libs/cscoreJNIShared/shared/linuxx86-64/release/:wpiutil/build/libs/wpiutil/shared/linuxx86-64/release/:wpiutil/build/libs/wpiutilJNIShared/shared/linuxx86-64/release/:hal/build/libs/hal/shared/linuxx86-64/release/:hal/build/libs/halJNIShared/shared/linuxx86-64/release/:$opencv_dir \
    java -jar myRobot/build/libs/myRobot-all.jar