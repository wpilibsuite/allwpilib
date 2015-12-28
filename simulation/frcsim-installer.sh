#!/bin/bash

function check-environment {
  # Ensure root access
  if [ "$(id -u)" != "0" ]; then
    echo "*** This script must be run as root!" 1>&2
    echo "*** Make sure that you followed the instructions properly." 1>&2
    install-fail
  fi

  # Make sure that we're on Ubuntu.
  if [ "$(lsb_release -is)" != "Ubuntu" ]; then
    if [ "$(lsb_release -is)" = "" ]; then
      echo "*** Distributions other than Ubuntu (such as yours, probably) are not supported." 1>&2
    else
      echo "*** Distributions other than Ubuntu (such as $(lsb_release -is)) are not supported." 1>&2
    fi
    echo "*** This means that the install will likely fail." 1>&2
    echo "*** Continue anyway? (y/n)" 1>&2
    read CONT
    if [ "$CONT" != "y" -a "$CONT" != "Y" ]; then
      install-fail
    fi
  fi

  # Make sure that we have /etc/apt/sources.list.d available.
  if [ ! -e /etc/apt/sources.list.d ]; then
    echo "*** Cannot find /etc/apt/sources.list.d - is apt installed?"
    install-fail
  fi

  # Make sure that apt-key is installed.
  if ! which apt-key >/dev/null; then
    echo "*** You don't appear to have apt-key installed." 1>&2
    echo "*** Please install apt and run the script again." 1>&2
    install-fail
  fi

  # Make sure that apt-get is installed.
  if ! which apt-get >/dev/null; then
    echo "*** You don't appear to have apt-get installed." 1>&2
    echo "*** Please install apt and run the script again." 1>&2
    install-fail
  fi

  # Make sure that we have internet access.
  if ! ping 8.8.8.8 -c 1 >/dev/null; then
    echo "*** You don't appear to be able to access the internet! (Can't ping 8.8.8.8)" 1>&2
    install-fail
  fi

  # Make sure that wget is installed.
  if ! which wget >/dev/null; then
    echo "*** You don't appear to have wget installed." 1>&2
    echo "*** Install? (y/n)" 1>&2
    read CONT
    if [ "$CONT" != "y" -a "$CONT" != "Y" ]; then
      install-fail
    fi
    apt-get install wget -y
  fi

  # Make sure that python is installed
  if ! which python >/dev/null; then
    echo "*** You don't appear to have python installed." 1>&2
    echo "*** Install? (y/n)" 1>&2
    read CONT
    if [ "$CONT" != "y" -a "$CONT" != "Y" ]; then
      install-fail
    fi
    apt-get install python -y
  fi

  # Make sure that unzip is installed
  if ! which unzip >/dev/null; then
    echo "*** You don't appear to have unzip installed." 1>&2
    echo "*** Install? (y/n)" 1>&2
    read CONT
    if [ "$CONT" != "y" -a "$CONT" != "Y" ]; then
      install-fail
    fi
    apt-get install unzip -y
  fi
}

function remove-frcsim {
  check-environment

  echo "*** Remove Gazebo package entry too? (y/n)" 1>&2
  read CONT
  if [ "$CONT" == "y" -o "$CONT" == "Y" ]; then
  rm -f /etc/apt/sources.list.d/gazebo-latest.list
  fi

  apt-get update
  apt-get remove --auto-remove gazebo6 g++-4.9 openjdk-8-jdk
  rm -rf /opt/eclipse
  rm -f /usr/share/applications/frcsim.desktop /usr/share/applications/eclipse.desktop /usr/share/applications/sim_ds.desktop
  rm -f /usr/bin/frcism /usr/bin/sim_ds /usr/bin/eclipse
  rm -rf ~/wpilib/simulation
}

function install-eclipse-plugins {
  if ! (wget -O /tmp/simulation.zip http://first.wpi.edu/FRC/roborio/maven/development/simulation/simulation/1.0.0/simulation-1.0.0.zip)
  then
    echo "***could not download wpilib simulation plugins, wrong URL probably***"
    install-fail
  fi

  mkdir -p ~/wpilib/simulation
  unzip /tmp/simulation.zip -d ~/wpilib/simulation
  ln -fs ~/wpilib/simulation/frcsim /usr/bin/frcsim
  ln -fs ~/wpilib/simulation/sim_ds /usr/bin/sim_ds
}

function install-eclipse {
  if ! (wget -O /tmp/eclipse.tar.gz http://eclipse.mirror.rafal.ca/technology/epp/downloads/release/mars/1/eclipse-cpp-mars-1-linux-gtk-x86_64.tar.gz)
  then
    echo "***could not download eclipse, wrong URL probably***"
    install-fail
  fi

  tar -xf /tmp/eclipse.tar.gz -C /opt
  ln -fs /opt/eclipse/eclipse /usr/bin/eclipse
}

function install-desktops {
  mv ~/wpilib/simulation/eclipse.desktop /usr/share/applications/eclipse.desktop
  mv ~/wpilib/simulation/frcsim.desktop /usr/share/applications/frcsim.desktop
  mv ~/wpilib/simulation/sim_ds.desktop /usr/share/applications/sim_ds.desktop
}

function install-gz_msgs {
  cd ~/wpilib/simulation/gz_msgs
  mkdir build
  cd build
  cmake ..
  make
  make install
}

function install-toolchain {
  add-apt-repository ppa:openjdk-r/ppa -y
  add-apt-repository ppa:ubuntu-toolchain-r/test -y
  apt-get update
  apt-get install cmake libprotoc-dev libprotobuf-dev protobuf-compiler g++-4.9 openjdk-8-jdk -y
  ln -fs /usr/bin/g++-4.9 /usr/bin/g++
}

function install-models {
  if ! (wget -O /tmp/models.zip https://usfirst.collab.net/sf/frs/do/downloadFile/projects.wpilib/frs.simulation.frcsim_gazebo_models/frs1160?)
  then
    echo "*** failed to download models. Check your internet connection! ***"
    install-fail
  fi

  unzip /tmp/models.zip -d /tmp
  mv /tmp/frcsim-gazebo-models/models ~/wpilib/simulation/models
  mv /tmp/frcsim-gazebo-models/worlds ~/wpilib/simulation/worlds
}

function install-frcsim {
  check-environment

  # Add Gazebo Repository and Key
  if ! echo "deb http://packages.osrfoundation.org/gazebo/ubuntu `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-latest.list
  then
    echo "*** Cannot add Gazebo repository!" 1>&2
    install-fail
  fi
  if ! (wget http://packages.osrfoundation.org/gazebo.key -O - | apt-key add -)
  then
    echo "*** Cannot add Gazebo repository key!" 1>&2
    if ! ping packages.osrfoundation.org -c 1 >/dev/null; then
      echo "*** The package host for Gazebo appears to be down. Try again later." 1>&2
    fi
    install-fail
  fi

  # Update and install dependencies
  if ! apt-get update
  then
    echo "*** Could not resynchronize package index files." 1>&2
    echo "*** Are you running another update or install?" 1>&2
    install-fail
  fi
  if ! apt-get install -y gazebo6
  then
    echo "*** Could not install frcsim packages. See above output for details." 1>&2
    echo "*** Are you running another update or install?" 1>&2
    install-fail
  fi

  install-eclipse-plugins
  install-toolchain
  install-gz_msgs
  install-eclipse
  install-desktops
  install-models

  sudo chown -R $USER:$USER ~/wpilib

  # Done
  echo "Installation Finished!!"
}

if [ "$1" == "ROOT" ]
then
  install-frcsim
elif [ "$1" == "INSTALLER" ]
then
  SUDO_ASKPASS=/usr/bin/ssh-askpass sudo bash -c "$0 ROOT"
elif [ "$1" == "REMOVE-ROOT" ]
then
  remove-frcsim
elif [ "$1" == "REMOVE" ]
then
  SUDO_ASKPASS=/usr/bin/ssh-askpass sudo bash -c "$0 REMOVE-ROOT"
else
  if [ -z "$1" ]
  then
    echo "***This script requires an argument!***"
    echo "***Run ./frcsim_installer.sh INSTALLER to install***"
    echo "***Other options are REMOVE, or any of the functions in the script"
  else
    $@
  fi
fi
