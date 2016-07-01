#!/bin/bash

if [[ $TRAVIS_OS_NAME != 'osx' ]]; then
    # Install custom requirements on Linux
    sudo add-apt-repository ppa:wpilib/toolchain -y
    sudo apt-get update -q
    sudo apt-get install frc-toolchain -y
fi
