#!/usr/bin/env bash
#*----------------------------------------------------------------------------*#
#* Copyright (c) FIRST 2014. All Rights Reserved.							  *#
#* Open Source Software - may be modified and shared by FRC teams. The code   *#
#* must be accompanied by the FIRST BSD license file in the root directory of *#
#* the project.															      *#
#*----------------------------------------------------------------------------*#

source config.sh

TESTSTAND_SERVE_COMMAND="/usr/local/frc/bin/teststand serve"

#Checks to see if the mutex driver station is running. If it isn't then start it.

SSH_CHECK_FOR_TESTSTAND="ssh -qn ${ADMIN_ROBOT_ADDRESS} 'pidof ${TESTSTAND_SERVE_COMMAND}' &>/dev/null"

TESTSTAND_SERVE_COMMAND="nohup ${TESTSTAND_SERVE_COMMAND} > /dev/null 2>&1 < /dev/null &"

if [ $(which sshpass) ]; then
	SSH_CHECK_FOR_TESTSTAND="sshpass -p '' ${SSH_CHECK_FOR_TESTSTAND}"
fi

echo $SSH_CHECK_FOR_TESTSTAND

if  $(eval $SSH_CHECK_FOR_TESTSTAND); then
	echo "Driver Station is already running";
	exit 0;
fi

echo "Starting Driver Station";

function startDS {
	SSH_START_TESTSTAND_DRIVER_STATION="ssh ${ADMIN_ROBOT_ADDRESS} ${TESTSTAND_SERVE_COMMAND}"
	if [ $(which sshpass) ]; then
		sshpass -p "" ${SSH_START_TESTSTAND_DRIVER_STATION}
	else
		eval ${SSH_START_TESTSTAND_DRIVER_STATION}
	fi
}

startDS&

sleep 1
