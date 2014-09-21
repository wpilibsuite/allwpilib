#!/usr/bin/env bash

# Update the apt-get repository list
apt --yes --force-yes install software-properties-common
apt-add-repository --yes ppa:byteit101/frc-toolchain
apt-get update


# Download and install Java
apt-get --yes --force-yes install default-jre default-jdk maven python-pip python-setuptools g++-arm-frc-linux-gnueabi sshpass libc6-i386

easy_install pip
yes | pip install git-review


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
