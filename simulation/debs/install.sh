#!/bin/bash

function install-frcsim {
    # Add Gazebo Repository and Key
    echo "deb http://packages.osrfoundation.org/gazebo/ubuntu `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-latest.list
    wget http://packages.osrfoundation.org/gazebo.key -O - | apt-key add -

    # Add FRCSim Repository and Key
    echo "deb http://users.wpi.edu/~adhenning/frcsim `lsb_release -cs` main" > /etc/apt/sources.list.d/frcsim-latest.list
    wget users.wpi.edu/~adhenning/frcsim.key -O - | apt-key add -

    # Update and install frcsim and its dependencies
    apt-get update
    apt-get install -y frcsim

    # Done
    python -c "raw_input(\"Installation Finished (Hit enter to exit) \")"
}

if [ "$1" == "ROOT" ]
then
    install-frcsim
elif [ "$1" == "INSTALLER" ]
then
    SUDO_ASKPASS=/usr/bin/ssh-askpass sudo bash -c "$0 ROOT"
else
    gnome-terminal --window --hide-menubar --title "FRCSim Installer" -e "$0 INSTALLER"
fi


