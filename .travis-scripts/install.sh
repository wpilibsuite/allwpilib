#!/bin/bash

if [[ $TRAVIS_OS_NAME != 'osx' ]]; then
    # Install custom requirements on Linux
    sudo sh -c 'echo "deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-5.0 main" > /etc/apt/sources.list.d/llvm.list'
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
    sudo add-apt-repository ppa:wpilib/toolchain -y
    sudo apt-get update -q
    sudo apt-get install frc-toolchain -y

    wget https://bootstrap.pypa.io/get-pip.py
    sudo python3.5 get-pip.py
    python3.5 -m pip install --user wpiformat
else
    brew update
    brew install python3 clang-format

    pip3 install --user wpiformat
fi
