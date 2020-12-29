#!/usr/bin/env bash
#*----------------------------------------------------------------------------*#
#* Copyright (c) 2014-2019 FIRST. All Rights Reserved.                        *#
#* Open Source Software - may be modified and shared by FRC teams. The code   *#
#* must be accompanied by the FIRST BSD license file in the root directory of *#
#* the project.                                                               *#
#*----------------------------------------------------------------------------*#

# This file is intended to be run in the DEFAULT_TEST_DIR on the roboRIO.
# Do not attempt to run this file on your local system.
# There is one file (delploy-and-run-test-on-robot.sh) that is designed to
# deploy this file allong with the compiled tests for you.

# Configurable variables
source config.sh

DEFAULT_TEST_DIR=${DEFAULT_DESTINATION_DIR}
DEFAULT_PATH_TO_JRE=/usr/local/frc/JRE/bin/java

usage="$(basename "$0") [-h] (java|cpp) [-d test_dir] [-A] [arg] [arg]...
A script designed to run the integration tests.
This script should only be run on the roborio.
Where:
    -h    Show this help text
    -d    The directory where the tests have been placed.
          This is done to prevent overwriting an already running program.
          This scrip will automatically move the test into the ${DEFAULT_TEST_DIR}
          directory.
          Default: Assumes the test is in the same directory as this scrip.
    -A    Do not use the default arguments for the given language.
    arg   The arguments to be passed to test."


# Are you trying to run this program on a platform other than the roboRIO?
if [[ ! -e "${DEFAULT_TEST_DIR}" ]]; then
    printf "Failed to find %s\nAre you trying to run this file on your local computer?\n" "${DEFAULT_TEST_DIR}"
    printf "This script should only be run on the roboRIO.\n\n"
    echo "$usage"
    exit 1
fi

LANGUAGE=none
TEST_FILE=none
TEST_DIR="$DEFAULT_TEST_DIR"
# Begin searching for options from the second paramater on
PARAM_ARGS=${@:2}
# Where the test arguments start
TEST_RUN_ARGS=${@:2}
RUN_WITH_DEFAULT_ARGS=true
DEFAULT_ARGS=""

# Determine the language that we are attempting to run
if [[ "$1" = java ]]; then
    LANGUAGE=$1
    TEST_FILE=$DEFAULT_JAVA_TEST_NAME
    DEFAULT_ARGS=$DEFAULT_JAVA_TEST_ARGS
elif [[ "$1" = cpp ]]; then
    LANGUAGE=$1
    TEST_FILE=$DEFAULT_CPP_TEST_NAME
    DEFAULT_ARGS=$DEFAULT_CPP_TEST_ARGS
elif [[ "$1" = "-h" ]]; then
    #If the first argument is the help option
    #Allow it to be searhced for in getopts
    PARAM_ARGS=${@}
else
    printf "Invalid language selection: %s\n\n" "$1" >&2
    echo "$usage" >&2
    exit 1
fi

PARAM_COUNTER=1
printf "Param Args ${PARAM_ARGS}\n"

# Check for optional parameters
while  getopts ':hmd:A' option $PARAM_ARGS ; do
    case "$option" in
    h)
        # Print the help message
        printf "Usage:\n"
        echo "$usage"
        exit
        ;;
    A)
        # Remove the default arguments
        RUN_WITH_DEFAULT_ARGS=false
        PARAM_COUNTER=$[$PARAM_COUNTER +1]
        ;;
    d)
        TEST_DIR=$OPTARG
        # Since we are selecting the directory the run args start from the 5th argument
        PARAM_COUNTER=$[$PARAM_COUNTER +2]
        ;;
    ?)
        # When an unknown param is found then we are done so break
        break
        ;;
    esac
done

TEST_RUN_ARGS=${@:$[$PARAM_COUNTER +1]}

if [[ "$RUN_WITH_DEFAULT_ARGS" == true ]]; then
    TEST_RUN_ARGS="${DEFAULT_ARGS} ${TEST_RUN_ARGS}"
fi

# Make sure at least two parameters are passed or four if running with -d option
if [[ $# -lt $PARAM_COUNTER ]]; then
    printf "Invalid arg count. Should be %s, was %s.\n" "${PARAM_COUNTER}" "$#"
    echo "$usage"
    exit 1
fi

# Make sure the webserver is disabled to save memory
/usr/local/natinst/etc/init.d/systemWebServer stop

# Kill all running robot programs
killall java FRCUserProgram

# If we are running with the -d argument move the test to the DEFAULT_TEST_DIR
if [[ ! -e "${TEST_DIR}/${TEST_FILE}" ]]; then
    printf "Failed to find %s.\nDid you copy the test file correctly?\n" "${TEST_DIR}/${TEST_FILE}"
    echo "$usage"
    exit 1
elif [[ $TEST_DIR != "$DEFAULT_TEST_DIR" ]]; then
    mv "${TEST_DIR}/${TEST_FILE}" "${DEFAULT_TEST_DIR}"
fi

# Make sure the excecutable file has permission to run

# Get the serial number and FPGADeviceCode for this rio
export serialnum=`/sbin/fw_printenv -n serial#`
export eval `/sbin/fw_printenv DeviceCode FPGADeviceCode DeviceDesc TargetClass`

# Store the run command for the language
RUN_COMMAND=none
if [[ ${LANGUAGE} = java ]]; then
    chmod a+x ${DEFAULT_JAVA_TEST_NAME}
    RUN_COMMAND="env LD_LIBRARY_PATH=/opt/GenICam_v3_0_NI/bin/Linux32_ARM/:/usr/local/frc/lib ${DEFAULT_PATH_TO_JRE} -ea -jar ${DEFAULT_JAVA_TEST_NAME} ${TEST_RUN_ARGS}"
elif [[ ${LANGUAGE} = cpp ]]; then
    chmod a+x ${DEFAULT_CPP_TEST_NAME}
    RUN_COMMAND="./${DEFAULT_CPP_TEST_NAME} ${TEST_RUN_ARGS}"
fi

printf "Running: %s\n\n" "${RUN_COMMAND}"
COREDUMP_DIR=${DEFAULT_DESTINATION_TEST_RESULTS_DIR}/coredump
mkdir -p ${COREDUMP_DIR}
CORE_LOCATION=${COREDUMP_DIR}/core
echo ${CORE_LOCATION} > /proc/sys/kernel/core_pattern
ulimit -c unlimited
eval ${RUN_COMMAND}
if [[ $? -gt 127 && -e ${CORE_LOCATION} ]]; then
    mv ${CORE_LOCATION} ${COREDUMP_DIR}/core.${LANGUAGE}
    if [[ ${LANGUAGE} = java ]]; then
        cp -p ${DEFAULT_JAVA_TEST_NAME} ${COREDUMP_DIR}/
    elif [[ ${LANGUAGE} = cpp ]]; then
        cp -p ${DEFAULT_CPP_TEST_NAME} ${COREDUMP_DIR}/
    fi
fi
