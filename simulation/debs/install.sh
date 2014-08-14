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
        echo "*** This means that if the install fails, don't blame us." 1>&2
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
        apt-get install wget
    fi
}

function install-fail {
    python -c "raw_input(\"Installation Failed (Hit enter to exit) \")" 1>&2
    exit 1
}

function remove-frcsim {

    check-environment

    echo "*** Remove Gazebo package entry too? (y/n)" 1>&2
    read CONT
    if [ "$CONT" == "y" -o "$CONT" == "Y" ]; then
        rm -f /etc/apt/sources.list.d/gazebo-latest.list
    fi
    rm -f /etc/apt/sources.list.d/frcsim-latest.list

    # TODO: Does not currently remove keys. Should it?

    apt-get update

    apt-get remove --auto-remove frcsim
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

    # Add FRCSim Repository and Key
    if ! echo "deb http://first.wpi.edu/FRC/roborio/release/linux `lsb_release -cs` main" > /etc/apt/sources.list.d/frcsim-latest.list
    then
        echo "*** Cannot add FRCSim repository!" 1>&2
        install-fail
    fi
    if ! (wget first.wpi.edu/FRC/roborio/wpilib.gpg.key -O - | apt-key add -)
    then
        echo "*** Cannot add FRCSim repository key!" 1>&2
        if ! ping first.wpi.edu -c 1 >/dev/null; then
            echo "*** The package host for FRCSim appears to be down. Try again later." 1>&2
        fi
        install-fail
    fi

    # Update and install frcsim and its dependencies
    if ! apt-get update
    then
        echo "*** Could not resynchronize package index files." 1>&2
        echo "*** Are you running another update or install?" 1>&2
        install-fail
    fi
    if ! apt-get install -y frcsim
    then
        echo "*** Could not install frcsim packages. See above output for details." 1>&2
        echo "*** Are you running another update or install?" 1>&2
        install-fail
    fi

    # Done
    python -c "raw_input(\"Installation Finished (Hit enter to exit) \")" 1>&2
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
    gnome-terminal --window --hide-menubar --title "FRCSim Installer" -e "$0 INSTALLER"
fi
