#!/usr/bin/env bash
#*----------------------------------------------------------------------------*#
#* Copyright (c) FIRST 2014. All Rights Reserved.							  *#
#* Open Source Software - may be modified and shared by FRC teams. The code   *#
#* must be accompanied by the FIRST BSD license file in the root directory of *#
#* the project.															      *#
#*----------------------------------------------------------------------------*#

# Configurable variables
source config.sh

# Java variables
DEFAULT_DESTINATION_JAVA_TEST_FILE=${DEFAULT_TEST_SCP_DIR}/${DEFAULT_JAVA_TEST_NAME}

# C++ Variables
DEFAULT_DESTINATION_CPP_TEST_FILE=${DEFAULT_TEST_SCP_DIR}/${DEFAULT_CPP_TEST_NAME}

DEFAULT_DESTINATION_RUN_TEST_SCRIPT=${DEFAULT_DESTINATION_DIR}/${DEFAULT_LOCAL_RUN_TEST_SCRIPT}

usage="$(basename "$0") [-h] (java|cpp) [-m] [-A] [arg] [arg]...
A script designed to run the integration tests.
This script should only be run on the roborio.
Where:
    -h    Show this help text.
    -m    The driver station mutex will be handled manually.
    -A    Disable language recomended arguments.
    arg   Additional arguments to be passed to test."


# These variables are set when the language is selected
LANGUAGE=none
LOCAL_TEST_FILE=none
DESTINATION_TEST_FILE=none
TEST_RUN_ARGS=""

# Begin searching for options from the third paramater on
PARAM_ARGS=${@:2}

if [[ "$1" = java ]]; then
	LANGUAGE=$1
	LOCAL_TEST_FILE=$DEFAULT_LOCAL_JAVA_TEST_FILE
	DESTINATION_TEST_FILE=$DEFAULT_DESTINATION_JAVA_TEST_FILE
elif [[ "$1" = cpp ]]; then
	LANGUAGE=$1
	LOCAL_TEST_FILE=$DEFAULT_LOCAL_CPP_TEST_FILE
	DESTINATION_TEST_FILE=$DEFAULT_DESTINATION_CPP_TEST_FILE
elif [[ "$1" = "-h" ]]; then
	printf "Usage:\n"
	echo "$usage"
	exit
else
	printf "Invalid language selection: %s\n\n" "$1" >&2
	echo "$usage" >&2
	exit 1
fi

# Check if the test file to upload exists
if [[ ! -e ${LOCAL_TEST_FILE} ]]; then
	printf "The test file does not exist: %s\nAre you sure that you compiled the tests??\n\n" "${LOCAL_TEST_FILE}" >&2
	echo "$usage" >&2
	exit 1
fi

MUTEX_OVERRIDE_PARAM_TEXT=""
if [[ "$2" = "-m" ]]; then
	MUTEX_OVERRIDE_PARAM_TEXT="-m "
	TEST_RUN_ARGS="${@:3}"
else
	TEST_RUN_ARGS="${@:2}"
fi

shopt -s huponexit

SCP_TEST_SCRIPT="scp config.sh ${DEFAULT_LOCAL_RUN_TEST_SCRIPT} ${ROBOT_ADDRESS}:/${DEFAULT_DESTINATION_DIR}"
SSH_CHMOD_AND_MAKE_TEMP_TEST_DIR="ssh -t ${ROBOT_ADDRESS} chmod a+x ${DEFAULT_DESTINATION_RUN_TEST_SCRIPT}; mkdir ${DEFAULT_TEST_SCP_DIR} 2>/dev/null"
SCP_TEST_PROGRAM="scp ${LOCAL_TEST_FILE} ${ROBOT_ADDRESS}:/${DESTINATION_TEST_FILE}"
SSH_RUN_TESTS="ssh -t ${ROBOT_ADDRESS} ${DEFAULT_DESTINATION_RUN_TEST_SCRIPT} ${LANGUAGE} $(whoami) ${MUTEX_OVERRIDE_PARAM_TEXT}-d ${DEFAULT_TEST_SCP_DIR} ${TEST_RUN_ARGS}"

if [ $(which sshpass) ]; then
	sshpass -p "" ${SCP_TEST_SCRIPT}
	sshpass -p "" ${SSH_CHMOD_AND_MAKE_TEMP_TEST_DIR}
	sshpass -p "" ${SCP_TEST_PROGRAM}
	sshpass -p "" ${SSH_RUN_TESTS}
else
	eval ${SCP_TEST_SCRIPT}
	eval ${SSH_CHMOD_AND_MAKE_TEMP_TEST_DIR}
	eval ${SCP_TEST_PROGRAM}
	eval ${SSH_RUN_TESTS}
fi
