#!/usr/bin/env bash
#*----------------------------------------------------------------------------*#
#* Copyright (c) FIRST 2014. All Rights Reserved.							  *#
#* Open Source Software - may be modified and shared by FRC teams. The code   *#
#* must be accompanied by the FIRST BSD license file in the root directory of *#
#* the project.															      *#
#*----------------------------------------------------------------------------*#

# This file is intended to be run in the DEFAULT_TEST_DIR on the roboRIO.
# Do not attempt to run this file on your local system.
# There is one file (delploy-and-run-test-on-robot.sh) that is designed to
# deploy this file allong with the compiled tests for you.

# Configurable variables
source config.sh

DEFAULT_TEST_DIR=${DEFAULT_DESTINATION_DIR}
DEFAULT_PATH_TO_JRE=/usr/local/frc/JRE/bin/java

usage="$(basename "$0") [-h] (java|cpp) name [-d test_dir] [-m] [-A] [arg] [arg]...
A script designed to run the integration tests.
This script should only be run on the roborio.
Where:
    -h    Show this help text
    name  The name of the user trying to run the tests (used for driver station)
    -d    The directory where the tests have been placed.
          This is done to prevent overwriting an already running program
          in the case where another user already has the driver station mutex.
          This scrip will automatically move the test into the ${DEFAULT_TEST_DIR}
          directory when the driver station mutex is released.
          Default: Assumes the test is in the same directory as this scrip.
    -m    The driver station mutex will be handled manually.
    -A    Do not use the default arguments for the given language.
    arg   The arguments to be passed to test."

mutexTaken=false
driverStationEnabled=false
# This function should run even if the script exits abnormally
function finish {
	if [ "$driverStationEnabled" == true ]; then
		/usr/local/frc/bin/teststand ds --name="${NAME}" disable
		driverStationEnabled=false
	fi
	if [ "$mutexTaken" == true ]; then
		/usr/local/frc/bin/teststand give --name="${NAME}"
		mutexTaken=false
	fi
}
trap finish EXIT SIGINT

# This function should be run asynchronysly to enable the tests 10
# seconds after they have been run.
function enableIn10Seconds {
	/usr/local/frc/bin/teststand ds --name="${NAME}" disable
	driverStationEnabled=true
	sleep 10
	/usr/local/frc/bin/teststand ds --name="${NAME}" enable
}

# Are you trying to run this program on a platform other than the roboRIO?
if [[ ! -e "${DEFAULT_TEST_DIR}" ]]; then
	printf "Failed to find %s\nAre you trying to run this file on your local computer?\n" "${DEFAULT_TEST_DIR}"
	printf "This script should only be run on the roboRIO.\n\n"
	echo "$usage"
	exit 1
fi

LANGUAGE=none
TEST_FILE=none
NAME=$2
TEST_DIR="$DEFAULT_TEST_DIR"
# Begin searching for options from the third paramater on
PARAM_ARGS=${@:3}
# Where the test arguments start
TEST_RUN_ARGS=${@:3}
RUN_WITH_DEFAULT_ARGS=true
DEFAULT_ARGS=""
MUTEX_OVERRIDE=false

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

PARAM_COUNTER=2
printf "Param Args ${PARAM_ARGS}\n"

# Check for optional paramaters
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
	m)
		MUTEX_OVERRIDE=true
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

# Make sure at least two paramaters are passed or four if running with -d option
if [[ $# -lt $PARAM_COUNTER ]]; then
	printf "Invalid arg count. Should be %s, was %s.\n" "${PARAM_COUNTER}" "$#"
	echo "$usage"
	exit 1
fi

# If the mutex has been retrived a higher level in the tree
if [ "$MUTEX_OVERRIDE" == false ]; then
	# Attempt to take the mutex for the driver station
	mutexTaken=true
	/usr/local/frc/bin/teststand take --name="${NAME}"
else
	printf "Override driver station control enabled.\n"
fi

# Kill all running robot programs
killall java FRCUserProgram

# Once we have the mutex no other tests are running
# If we are running with the -d argument move the test to the DEFAULT_TEST_DIR
if [[ ! -e "${TEST_DIR}/${TEST_FILE}" ]]; then
	printf "Failed to find %s.\nDid you copy the test file correctly?\n" "${TEST_DIR}/${TEST_FILE}"
	echo "$usage"
	exit 1
elif [[ $TEST_DIR != "$DEFAULT_TEST_DIR" ]]; then
	mv "${TEST_DIR}/${TEST_FILE}" "${DEFAULT_TEST_DIR}"
fi

# Make sure the excecutable file has permission to run

# Setup the driver station to enable automatically in 10 seconds without waiting for the function to excecute.
enableIn10Seconds&

# Store the run command for the language
RUN_COMMAND=none
if [[ ${LANGUAGE} = java ]]; then
	chmod a+x ${DEFAULT_JAVA_TEST_NAME}
	RUN_COMMAND="env LD_LIBRARY_PATH=/usr/local/frc/rpath-lib/ ${DEFAULT_PATH_TO_JRE} -ea -jar ${DEFAULT_JAVA_TEST_NAME} ${TEST_RUN_ARGS}"
elif [[ ${LANGUAGE} = cpp ]]; then
	chmod a+x ${DEFAULT_CPP_TEST_NAME}
	RUN_COMMAND="./${DEFAULT_CPP_TEST_NAME} ${TEST_RUN_ARGS}"
fi

printf "Running: %s\n\n" "${RUN_COMMAND}"
eval ${RUN_COMMAND}
