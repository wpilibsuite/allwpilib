#!/usr/bin/env bash

# Update the apt-get repository list
apt-get update


# Download and install Java
apt-get --yes --force-yes install default-jre
apt-get --yes --force-yes install default-jdk


# Install Maven
apt-get --yes --force-yes install maven


# Install git-review
apt-get --yes --force-yes install python-pip
apt-get --yes --force-yes install python-setuptools
easy_install pip
yes | pip install git-review


# Download and install the gcc linux arm-toolchains
# Get the 32 bit c library
apt-get --yes --force-yes install libc6-i386
# Download the tool to unzip .zip files
apt-get --yes --force-yes install unzip
# The variables associated with the toolchains
TOOL_VER="1.0.1"
TOOL_DIR=~/../home/vagrant/wpilib/toolchains
# Create the toolchains directory
mkdir -p $TOOL_DIR
# Download the toolchains to this directory
wget -P ${TOOL_DIR}/ http://first.wpi.edu/FRC/c/maven/edu/wpi/first/wpilib/plugins/cpp/toolchains/linux/${TOOL_VER}/linux-${TOOL_VER}.zip
# Unzip the file
unzip -q ${TOOL_DIR}/linux-${TOOL_VER}.zip -d ${TOOL_DIR}/
# Remove the unneeded zip file
rm ${TOOL_DIR}/linux-${TOOL_VER}.zip
# Change the name of the directory
mv -f $TOOL_DIR/linux-${TOOL_VER} ${TOOL_DIR}/arm-none-linux-gnueabi-4.4.1
chown -R vagrant $TOOL_DIR/arm-none-linux-gnueabi-4.4.1
# Add the arm toolchains to the path variable
echo 'PATH=$PATH:/home/vagrant/wpilib/toolchains/arm-none-linux-gnueabi-4.4.1/bin/' >> /home/vagrant/.bashrc


# Install the jdk-linux-arm-vfp-sflt
# Create the target directory for the jdk
mkdir -p /home/vagrant/jdk-linux-arm-vfp-sflt/
# Unzip the jdk into this new directory
tar -xzf /vagrant/jdk-7u45-linux-arm-vfp-sflt.gz -C /home/vagrant/jdk-linux-arm-vfp-sflt/

# Keep this at the end of this file
printf "\n\n"
printf "Your virtual enviroment is now nearly set-up.\n"
printf "You can access your enviroment using 'vagrant ssh'.\n"
printf "Please run 'cd /vagrant' to navigate to the repository.\n"
printf "Once you have navigated there please run 'git review -s' to complete the setup of git review.\n"
