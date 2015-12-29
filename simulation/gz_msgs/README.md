Building gz_msgs
================

gz_msgs currently uses cmake, is built on the end-users computer.
This allows us to support various versions of protobuf (2.5 on Ubuntu 14.04, 2.6 on Ubuntu 15.10)

To build, run the following commands in the ~/wpilib/simulation/gz_msgs directory.
If that directory doesn't exist, it means you haven't installed correctly.

If you're a developer for wpilib, you will need to unzip the simulation.zip file you published into ~/releases.
If you are a student using FRCSim, you should have downloaded that zip when you installed frcsim.

    mkdir build
    cd build
    cmake ..
    make
    make install

If you are installing FRCSim with the script, then this *should* have be done for you.
