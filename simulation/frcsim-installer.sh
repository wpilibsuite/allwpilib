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

  # Make sure that app-apt-repository is installed
  if ! which add-apt-repository >/dev/null; then
    echo "*** You don't appear to have all apt commands installed." 1>&2
    echo "*** Install? (y/n)" 1>&2
    read CONT
    if [ "$CONT" != "y" -a "$CONT" != "Y" ]; then
      install-fail
    fi
    apt-get install software-properties-common -y
  fi
}

function remove-frcsim {
  echo "*** Remove Gazebo package entry too? (y/n)" 1>&2
  read CONT
  if [ "$CONT" == "y" -o "$CONT" == "Y" ]; then
  rm -f /etc/apt/sources.list.d/gazebo-latest.list
  fi

  apt-get remove --auto-remove libgazebo6-dev gazebo6 g++-4.9 openjdk-8-jdk
  rm -rf /opt/eclipse
  rm -f /usr/share/applications/frcsim.desktop /usr/share/applications/eclipse.desktop /usr/share/applications/sim_ds.desktop
  rm -f /usr/bin/frcism /usr/bin/sim_ds /usr/bin/eclipse
  rm -rf ~/wpilib/simulation

  add-apt-repository --remove ppa:openjdk-r/ppa -y
  add-apt-repository --remove ppa:ubuntu-toolchain-r/test -y
}

function install-eclipse-plugins {
  #valid URLs can have promotion status of any of the following
  # - development (used for most recent merge into wpilib)
  # - beta
  # - release
  # - stable

  #this file is published to maven repo by simulation/build.gradle
  if ! (wget -O /tmp/simulation.zip http://first.wpi.edu/FRC/roborio/maven/$PROMOTION_STATUS/edu/wpi/first/wpilib/simulation/simulation/1.0.0/simulation-1.0.0.zip)
  then
    echo "***could not download wpilib simulation plugins, wrong URL probably***"
    echo "promotion status = $PROMOTION_STATUS"
    echo "url = http://first.wpi.edu/FRC/roborio/maven/$PROMOTION_STATUS/edu/wpi/first/wpilib/simulation/simulation/1.0.0/simulation-1.0.0.zip"
    install-fail
  fi

  mkdir -p ~/wpilib/simulation
  unzip /tmp/simulation.zip -d ~/wpilib/simulation
  rm -f /usr/bin/frcsim /usr/bin/sim_ds
  ln -s ~/wpilib/simulation/frcsim /usr/bin/frcsim
  ln -s ~/wpilib/simulation/sim_ds /usr/bin/sim_ds
}

function install-eclipse {
  if ! (wget -O /tmp/eclipse.tar.gz http://eclipse.mirror.rafal.ca/technology/epp/downloads/release/mars/1/eclipse-cpp-mars-1-linux-gtk-x86_64.tar.gz)
  then
    echo "***could not download eclipse, wrong URL probably***"
    install-fail
  fi

  tar -xf /tmp/eclipse.tar.gz -C /opt
  rm -f /usr/bin/eclipse
  ln -s /opt/eclipse/eclipse /usr/bin/eclipse
}

function install-desktops {
  # desktop files allow ubuntu (unity) users to "search" for their programs in the sidebar
  mv ~/wpilib/simulation/eclipse.desktop /usr/share/applications/eclipse.desktop
  mv ~/wpilib/simulation/frcsim.desktop /usr/share/applications/frcsim.desktop
  mv ~/wpilib/simulation/sim_ds.desktop /usr/share/applications/sim_ds.desktop
  mkdir -p /usr/share/icons/sim_ds
  mv ~/wpilib/simulation/sim_ds_logo.png /usr/share/icons/sim_ds/sim_ds_logo.png
}

function install-gz_msgs {
  # gz_msgs is built on the end-user system
  # that way the versions of protobuf will match whatever the default for that platform is
  cd ~/wpilib/simulation/gz_msgs
  mkdir build
  cd build
  cmake ..
  make
  make install
  chmod u+x ~/wpilib/simulation/lib/libgz_msgs.so
}

function install-toolchain {
  # older version of ubuntu like 14.04 don't have the versions of g++ and java we need
  # we can add some very reliable PPAs to get them however
  if [[ "`lsb_release -rs`" == "14.04" ]]
  then
    echo "*** You're using `lsb_release -r`, you will need additional repositories***"
    echo "*** Install? (y/n)" 1>&2
    read CONT
    if [ "$CONT" != "y" -a "$CONT" != "Y" ]; then
      install-fail
    fi
    add-apt-repository ppa:openjdk-r/ppa -y
    add-apt-repository ppa:ubuntu-toolchain-r/test -y
  fi
  if [[ "`lsb_release -rs`" == "15.04" ]]
  then
    echo "*** You're using `lsb_release -r`, you will need additional repositories***"
    echo "*** Install? (y/n)" 1>&2
    read CONT
    if [ "$CONT" != "y" -a "$CONT" != "Y" ]; then
      install-fail
    fi
    add-apt-repository ppa:openjdk-r/ppa -y
  fi

  # Update and install dependencies
  if ! apt-get update
  then
    echo "*** Could not resynchronize package index files." 1>&2
    echo "*** Are you running another update or install?" 1>&2
    install-fail
  fi

  apt-get install cmake libprotoc-dev libprotobuf-dev protobuf-compiler g++-4.9 openjdk-8-jdk -y
  rm -f /usr/bin/g++
  ln -s /usr/bin/g++-4.9 /usr/bin/g++
}

function install-models {
  # this zip file is made by hand. A better option to add models is to use the gazebo repository
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

  install-toolchain

  if ! apt-get install -y libgazebo6-dev gazebo6
  then
    echo "*** Could not install frcsim packages. See above output for details." 1>&2
    echo "*** Are you running another update or install?" 1>&2
    install-fail
  fi

  install-eclipse-plugins
  install-gz_msgs
  install-eclipse
  install-desktops
  install-models

  change-ownership

  # Done
  echo "Installation Finished!!"
}

function install-fail {
  echo "***INSTALLATION UNSUCCESSFUL***"
  echo "***Check the output about for anything that looks like errors***"
  echo "Please comment on the following to tutorial if you're unable to resolve your problem:"
  echo "https://wpilib.screenstepslive.com/s/4485/m/23353/l/478421-installing-frcsim-with-a-script-ubuntu"
  exit 1
}

function change-ownership {
  chown -R $NON_SUDO_USER:$NON_SUDO_USER ~/wpilib
}

if [ "$1" == "INSTALL-ROOT" ]
then

  if [ -z "$2" ]
  then
    echo "*** Could not set user ~/wpilib ownership to empty user***"
    install-fail
  else
    NON_SUDO_USER="$2"
  fi

  if [ -z "$3" ]
  then
    PROMOTION_STATUS="release"
  else
    PROMOTION_STATUS="$3"
  fi

  install-frcsim

elif [ "$1" == "INSTALL" ]
then
  NON_SUDO_USER="$USER"
  PROMOTION_STATUS="$2"
  SUDO_ASKPASS=/usr/bin/ssh-askpass sudo bash -c "$0 INSTALL-ROOT $NON_SUDO_USER $PROMOTION_STATUS"
elif [ "$1" == "REMOVE-ROOT" ]
then
  remove-frcsim
elif [ "$1" == "REMOVE" ]
then
  NON_SUDO_USER="$USER"
  SUDO_ASKPASS=/usr/bin/ssh-askpass sudo bash -c "$0 REMOVE-ROOT $NON_SUDO_USER $PROMOTION_STATUS"
else
  echo "***This script requires an argument!***"
  echo "***Run ./frcsim_installer.sh INSTALL to install***"
  echo "***The other option is REMOVE"
fi
