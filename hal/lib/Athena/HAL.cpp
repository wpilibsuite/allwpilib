#include "HAL/HAL.hpp"

#include "Port.h"
#include "HAL/Errors.hpp"
#include "ChipObject.h"
#include "NetworkCommunication/FRCComm.h"
#include "NetworkCommunication/UsageReporting.h"
#include "NetworkCommunication/LoadOut.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <signal.h> // linux for kill
const uint32_t solenoid_kNumDO7_0Elements = 8;
const uint32_t dio_kNumSystems = tDIO::kNumSystems;
const uint32_t interrupt_kNumSystems = tInterrupt::kNumSystems;
const uint32_t kSystemClockTicksPerMicrosecond = 40;

static tGlobal *global;

void* getPort(uint8_t pin)
{
	Port* port = new Port();
	port->pin = pin;
	port->module = 1;
	return port;
}

/**
 * @deprecated Uses module numbers
 */
void* getPortWithModule(uint8_t module, uint8_t pin)
{
	Port* port = new Port();
	port->pin = pin;
	port->module = module;
	return port;
}

const char* getHALErrorMessage(int32_t code)
{
	if (code == 0)
		return "";
	else if (code == SAMPLE_RATE_TOO_HIGH)
		return SAMPLE_RATE_TOO_HIGH_MESSAGE;
	else if (code == VOLTAGE_OUT_OF_RANGE)
		return VOLTAGE_OUT_OF_RANGE_MESSAGE;
	else if (code == LOOP_TIMING_ERROR)
		return LOOP_TIMING_ERROR_MESSAGE;
	else if (code == SPI_WRITE_NO_MOSI)
		return SPI_WRITE_NO_MOSI_MESSAGE;
	else if (code == SPI_READ_NO_MISO)
		return SPI_READ_NO_MISO_MESSAGE;
	else if (code == SPI_READ_NO_DATA)
		return SPI_READ_NO_DATA_MESSAGE;
	else if (code == INCOMPATIBLE_STATE)
		return INCOMPATIBLE_STATE_MESSAGE;
	else if (code == NO_AVAILABLE_RESOURCES)
		return NO_AVAILABLE_RESOURCES_MESSAGE;
	else if (code == NULL_PARAMETER)
		return NULL_PARAMETER_MESSAGE;
	else if (code == ANALOG_TRIGGER_LIMIT_ORDER_ERROR)
		return ANALOG_TRIGGER_LIMIT_ORDER_ERROR_MESSAGE;
	else if (code == ANALOG_TRIGGER_PULSE_OUTPUT_ERROR)
		return ANALOG_TRIGGER_PULSE_OUTPUT_ERROR_MESSAGE;
	else if (code == PARAMETER_OUT_OF_RANGE)
		return PARAMETER_OUT_OF_RANGE_MESSAGE;
	else
		return "";
}

/**
 * Return the FPGA Version number.
 * For now, expect this to be competition year.
 * @return FPGA Version number.
 */
uint16_t getFPGAVersion(int32_t *status)
{
	return global->readVersion(status);
}

/**
 * Return the FPGA Revision number.
 * The format of the revision is 3 numbers.
 * The 12 most significant bits are the Major Revision.
 * the next 8 bits are the Minor Revision.
 * The 12 least significant bits are the Build Number.
 * @return FPGA Revision number.
 */
uint32_t getFPGARevision(int32_t *status)
{
	return global->readRevision(status);
}

/**
 * Read the microsecond-resolution timer on the FPGA.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA reset).
 */
uint32_t getFPGATime(int32_t *status)
{
	return global->readLocalTime(status);
}

/**
 * Get the state of the "USER" button on the RoboRIO
 * @return true if the button is currently pressed down
 */
bool getFPGAButton(int32_t *status)
{
	return global->readUserButton(status);
}

int HALSetErrorData(const char *errors, int errorsLength, int wait_ms)
{
	return setErrorData(errors, errorsLength, wait_ms);
}

int HALGetControlWord(HALControlWord *data)
{
	return FRC_NetworkCommunication_getControlWord((ControlWord_t*) data);
}

int HALGetAllianceStation(enum HALAllianceStationID *allianceStation)
{
	return FRC_NetworkCommunication_getAllianceStation((AllianceStationID_t*) allianceStation);
}

int HALGetJoystickAxes(uint8_t joystickNum, HALJoystickAxes *axes, uint8_t maxAxes)
{
	return FRC_NetworkCommunication_getJoystickAxes(joystickNum, (JoystickAxes_t*) axes, maxAxes);
}

int HALGetJoystickButtons(uint8_t joystickNum, HALJoystickButtons *buttons, uint8_t *count)
{
	return FRC_NetworkCommunication_getJoystickButtons(joystickNum, buttons, count);
}

void HALSetNewDataSem(pthread_mutex_t * param)
{
	setNewDataSem(param);
}

/**
 * Call this to start up HAL. This is required for robot programs.
 */
int HALInitialize(int mode)
{
	FRC_NetworkCommunication_Reserve(nullptr);
	// image 4; Fixes errors caused by multiple processes. Talk to NI about this
	nFPGA::nRoboRIO_FPGANamespace::g_currentTargetClass =
			nLoadOut::kTargetClass_RoboRIO;

	int32_t status;
	global = tGlobal::create(&status);

	// Kill any previous robot programs
	std::fstream fs;
	// By making this both in/out, it won't give us an error if it doesnt exist
	fs.open("/var/lock/frc.pid", std::fstream::in | std::fstream::out);
	if (fs.bad())
		return 0;

	pid_t pid = 0;
	if (!fs.eof() && !fs.fail())
	{
		fs >> pid;
		//see if the pid is around, but we don't want to mess with init id=1, or ourselves
		if (pid >= 2 && kill(pid, 0) == 0 && pid != getpid())
		{
			std::cout << "Killing previously running FRC program..."
					<< std::endl;
			kill(pid, SIGTERM); // try to kill it
			delayMillis(100);
			if (kill(pid, 0) == 0)
			{
				// still not successfull
				if (mode == 0)
				{
					std::cout << "FRC pid " << pid
							<< " did not die within 110ms. Aborting"
							<< std::endl;
					return 0; // just fail
				}
				else if (mode == 1) // kill -9 it
					kill(pid, SIGKILL);
				else
				{
					std::cout << "WARNING: FRC pid " << pid
							<< " did not die within 110ms." << std::endl;
				}
			}

		}
	}
	fs.close();
	// we will re-open it write only to truncate the file
	fs.open("/var/lock/frc.pid", std::fstream::out | std::fstream::trunc);
	fs.seekp(0);
	pid = getpid();
	fs << pid << std::endl;
	fs.close();
	return 1;
}

void HALNetworkCommunicationObserveUserProgramStarting(void)
{
	FRC_NetworkCommunication_observeUserProgramStarting();
}

void HALNetworkCommunicationObserveUserProgramDisabled(void)
{
	FRC_NetworkCommunication_observeUserProgramDisabled();
}

void HALNetworkCommunicationObserveUserProgramAutonomous(void)
{
	FRC_NetworkCommunication_observeUserProgramAutonomous();
}

void HALNetworkCommunicationObserveUserProgramTeleop(void)
{
	FRC_NetworkCommunication_observeUserProgramTeleop();
}

void HALNetworkCommunicationObserveUserProgramTest(void)
{
	FRC_NetworkCommunication_observeUserProgramTest();
}

uint32_t HALReport(uint8_t resource, uint8_t instanceNumber, uint8_t context,
		const char *feature)
{
	if(feature == NULL)
	{
		feature = "";
	}

	return FRC_NetworkCommunication_nUsageReporting_report(resource, instanceNumber, context, feature);
}

// TODO: HACKS
void NumericArrayResize()
{
}
void RTSetCleanupProc()
{
}
void EDVR_CreateReference()
{
}
void Occur()
{
}

void imaqGetErrorText()
{
}
void imaqGetLastError()
{
}
void niTimestamp64()
{
}
