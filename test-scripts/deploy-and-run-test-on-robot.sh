#!/usr/bin/env bash
#*----------------------------------------------------------------------------*#
#* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        *#
#* Open Source Software - may be modified and shared by FRC teams. The code   *#
#* must be accompanied by the FIRST BSD license file in the root directory of *#
#* the project.                                                               *#
#*----------------------------------------------------------------------------*#

# Configurable variables
source config.sh

# Java variables
DEFAULT_DESTINATION_JAVA_TEST_FILE=${DEFAULT_TEST_SCP_DIR}/${DEFAULT_JAVA_TEST_NAME}

# C++ Variables
DEFAULT_DESTINATION_CPP_TEST_FILE=${DEFAULT_TEST_SCP_DIR}/${DEFAULT_CPP_TEST_NAME}

DEFAULT_DESTINATION_RUN_TEST_SCRIPT=${DEFAULT_DESTINATION_DIR}/${DEFAULT_LOCAL_RUN_TEST_SCRIPT}

usage="$(basename "$0") [-h] (java|cpp) [-A] [arg] [arg]...
A script designed to run the integration tests.
This script should only be run on the computer connected to the roboRIO.
Where:
    -h    Show this help text.
    -A    Disable language recommended arguments.
    arg   Additional arguments to be passed to test."


# These variables are set when the language is selected
LANGUAGE=none
LOCAL_TEST_FILE=none
DESTINATION_TEST_FILE=none
TEST_RUN_ARGS=""
DESTINATION_TEST_RESULTS=none
LOCAL_TEST_RESULTS=none


# Begin searching for options from the second paramater on
PARAM_ARGS=${@:2}

if [[ "$1" = java ]]; then
    LANGUAGE=$1
    LOCAL_TEST_FILE=$DEFAULT_LOCAL_JAVA_TEST_FILE
    DESTINATION_TEST_FILE=$DEFAULT_DESTINATION_JAVA_TEST_FILE
    DESTINATION_TEST_RESULTS=$DEFAULT_DESTINATION_JAVA_TEST_RESULTS
    LOCAL_TEST_RESULTS=$DEFAULT_LOCAL_JAVA_TEST_RESULT
elif [[ "$1" = cpp ]]; then
    LANGUAGE=$1
    LOCAL_TEST_FILE=$DEFAULT_LOCAL_CPP_TEST_FILE
    DESTINATION_TEST_FILE=$DEFAULT_DESTINATION_CPP_TEST_FILE
    DESTINATION_TEST_RESULTS=$DEFAULT_DESTINATION_CPP_TEST_RESULTS
    LOCAL_TEST_RESULTS=$DEFAULT_LOCAL_CPP_TEST_RESULT
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

TEST_RUN_ARGS="${@:2}"

shopt -s huponexit

# Fail if any command fails
set -e

ssh ${ROBOT_ADDRESS} "rm -R ${DEFAULT_DESTINATION_TEST_RESULTS_DIR}; mkdir ${DEFAULT_DESTINATION_TEST_RESULTS_DIR}"
scp ${DEFAULT_LIBRARY_NATIVE_FILES}/* ${ROBOT_ADDRESS}:${DEFAULT_LIBRARY_NATIVE_DESTINATION}
ssh ${ADMIN_ROBOT_ADDRESS} ldconfig
scp config.sh ${DEFAULT_LOCAL_RUN_TEST_SCRIPT} ${ROBOT_ADDRESS}:/${DEFAULT_DESTINATION_DIR}
ssh ${ROBOT_ADDRESS} "chmod a+x ${DEFAULT_DESTINATION_RUN_TEST_SCRIPT}; mkdir ${DEFAULT_TEST_SCP_DIR}; touch ${DESTINATION_TEST_FILE}"
scp ${LOCAL_TEST_FILE} ${ROBOT_ADDRESS}:${DESTINATION_TEST_FILE}
ssh ${ROBOT_ADDRESS} ${DEFAULT_DESTINATION_RUN_TEST_SCRIPT} ${LANGUAGE} -d ${DEFAULT_TEST_SCP_DIR} ${TEST_RUN_ARGS}
mkdir ${DEFAULT_LOCAL_TEST_RESULTS_DIR}; scp ${ROBOT_ADDRESS}:${DESTINATION_TEST_RESULTS} ${LOCAL_TEST_RESULTS}
