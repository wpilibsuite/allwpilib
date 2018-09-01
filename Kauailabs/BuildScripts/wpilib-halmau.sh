#!/bin/bash

set -x

cd ~
git clone https://github.com/kauailabs/allwpilib.git

cd allwpilib/
git checkout Mau.DylanLaMarca
git checkout

#Select from here until next comment for rebuilding the .so's
cd ~
sudo rm -r wpilib_build
mkdir wpilib_build/
cd wpilib_build
export JAVA_HOME=/usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt
cmake -D WITHOUT_ALLWPILIB=OFF -D WITHOUT_JAVA=ON -DCMAKE_BUILD_TYPE=Debug -D OpenCV_DIR=/home/pi/opencv-3.3.1/build ~/allwpilib
make -j5
sudo make install
#Run the commands between these two comments to rebuild the .so's

sudo cp -r ~/allwpilib/hal/src/main/native/mau/Translator/Maps /usr/local/wpilib/lib
sudo mkdir /tmp/frc_versions