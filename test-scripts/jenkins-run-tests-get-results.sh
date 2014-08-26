#!/usr/bin/env bash
#*----------------------------------------------------------------------------*#
#* Copyright (c) FIRST 2014. All Rights Reserved.							  *#
#* Open Source Software - may be modified and shared by FRC teams. The code   *#
#* must be accompanied by the FIRST BSD license file in the root directory of *#
#* the project.															      *#
#*----------------------------------------------------------------------------*#

# Configurable variables
source config.sh

# If there are already test results in the repository then remove them
if [[ -e ${DEFAULT_LOCAL_TEST_RESULTS_DIR} ]]; then
	rm -R ${DEFAULT_LOCAL_TEST_RESULTS_DIR}
fi

# Make the directory where the tests should live
mkdir ${DEFAULT_LOCAL_TEST_RESULTS_DIR} 2>/dev/null

# Remove the preivous test results from the the robot
SSH_REMOVE_OLD_TEST_RESULTS="ssh -t ${ROBOT_ADDRESS} 'rm -R ${DEFAULT_DESTINATION_TEST_RESULTS_DIR}; mkdir ${DEFAULT_DESTINATION_TEST_RESULTS_DIR}'"
if [ $(which sshpass) ]; then
	sshpass -p "" ${SSH_REMOVE_OLD_TEST_RESULTS}
else
	eval ${SSH_REMOVE_OLD_TEST_RESULTS}
fi

printf "Running cpp test\n"

# Run the C++ Tests
deploy-and-run-test-on-robot.sh cpp "--gtest_output=xml:${DEFAULT_DESTINATION_CPP_TEST_RESULTS}"

# Retrive the C++ Test Results
SCP_GET_CPP_TEST_RESULT="scp ${ROBOT_ADDRESS}:${DEFAULT_DESTINATION_CPP_TEST_RESULTS} ${DEFAULT_LOCAL_CPP_TEST_RESULT}"
if [ $(which sshpass) ]; then
	sshpass -p "" ${SCP_GET_CPP_TEST_RESULT}
else
	eval ${SCP_GET_CPP_TEST_RESULT}
fi

# Run the Java Tests
deploy-and-run-test-on-robot.sh java

# Retrive the Java Test Results
SCP_GET_JAVA_TEST_RESULT="scp ${ROBOT_ADDRESS}:${DEFAULT_DESTINATION_JAVA_TEST_RESULTS} ${DEFAULT_LOCAL_JAVA_TEST_RESULT}"
if [ $(which sshpass) ]; then
	sshpass -p "" ${SCP_GET_JAVA_TEST_RESULT}
else
	eval ${SCP_GET_JAVA_TEST_RESULT}
fi
