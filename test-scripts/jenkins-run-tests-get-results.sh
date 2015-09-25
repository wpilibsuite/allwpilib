#!/usr/bin/env bash
#*----------------------------------------------------------------------------*#
#* Copyright (c) FIRST 2014. All Rights Reserved.							  *#
#* Open Source Software - may be modified and shared by FRC teams. The code   *#
#* must be accompanied by the FIRST BSD license file in the root directory of *#
#* the project.															      *#
#*----------------------------------------------------------------------------*#

# Configurable variables
source config.sh

# Setup the mutex release before we grab it
mutexTaken=false
# This function should run even if the script exits abnormally
function finish {
	if [ "$mutexTaken" == true ]; then
		SSH_GIVE_MUTEX="ssh -t ${ROBOT_ADDRESS} /usr/local/frc/bin/teststand give --name=$(whoami)"
		if [ $(which sshpass) ]; then
			sshpass -p "" ${SSH_GIVE_MUTEX}
		else
			printf "WARNING!!! THIS IS HOW THE MUTEX IS RELEASED!\nIF YOU CHOOSE TO 'ctr+c' NOW YOU WILL HAVE TO HAND BACK THE MUTEX MANUALLY ON THE ROBOT.\n"
			eval ${SSH_GIVE_MUTEX}
		fi
		mutexTaken=false
	fi
}
trap finish EXIT SIGINT



# Take the mutex from the driver station
mutexTaken=true
SSH_TAKE_MUTEX="ssh -t ${ROBOT_ADDRESS} /usr/local/frc/bin/teststand take --name=$(whoami)"
if [ $(which sshpass) ]; then
	sshpass -p "" ${SSH_TAKE_MUTEX}
else
	eval ${SSH_TAKE_MUTEX}
fi

# If there are already test results in the repository then remove them
if [[ -e ${DEFAULT_LOCAL_TEST_RESULTS_DIR} ]]; then
	rm -R ${DEFAULT_LOCAL_TEST_RESULTS_DIR}
fi

# Make the directory where the tests should live
mkdir ${DEFAULT_LOCAL_TEST_RESULTS_DIR} 2>/dev/null

# Remove the preivous test results from the the robot
SSH_REMOVE_OLD_TEST_RESULTS="ssh -t ${ROBOT_ADDRESS} rm -R ${DEFAULT_DESTINATION_TEST_RESULTS_DIR}; mkdir ${DEFAULT_DESTINATION_TEST_RESULTS_DIR}"
if [ $(which sshpass) ]; then
	sshpass -p "" ${SSH_REMOVE_OLD_TEST_RESULTS}
else
	eval ${SSH_REMOVE_OLD_TEST_RESULTS}
fi

printf "Running cpp test\n"

# Run the C++ Tests
./deploy-and-run-test-on-robot.sh cpp -m -A "--gtest_output=xml:${DEFAULT_DESTINATION_CPP_TEST_RESULTS}"

# Retrive the C++ Test Results
SCP_GET_CPP_TEST_RESULT="scp ${ROBOT_ADDRESS}:${DEFAULT_DESTINATION_CPP_TEST_RESULTS} ${DEFAULT_LOCAL_CPP_TEST_RESULT}"
if [ $(which sshpass) ]; then
	sshpass -p "" ${SCP_GET_CPP_TEST_RESULT}
else
	eval ${SCP_GET_CPP_TEST_RESULT}
fi

# Run the Java Tests
./deploy-and-run-test-on-robot.sh java -m

# Retrive the Java Test Results
SCP_GET_JAVA_TEST_RESULT="scp ${ROBOT_ADDRESS}:${DEFAULT_DESTINATION_JAVA_TEST_RESULTS} ${DEFAULT_LOCAL_JAVA_TEST_RESULT}"
if [ $(which sshpass) ]; then
	sshpass -p "" ${SCP_GET_JAVA_TEST_RESULT}
else
	eval ${SCP_GET_JAVA_TEST_RESULT}
fi

# Make sure that we got test results back.
if [ ! -e ${DEFAULT_LOCAL_CPP_TEST_RESULT} ]; then
  echo "There are no results from the C++ tests; they must have failed."
  exit 100
fi

if [ ! -e ${DEFAULT_LOCAL_JAVA_TEST_RESULT} ]; then
  echo "There are no results from the Java tests; they must have failed."
  exit 101
fi

# The mutex is released when this program exits
