#include "HAL/HAL.hpp"

#include "Port.h"
#include "HAL/Errors.hpp"
#include "ChipObject.h"
#include "NetworkCommunication/FRCComm.h"
#include "NetworkCommunication/UsageReporting.h"
#include "NetworkCommunication/LoadOut.h"
#include "ChipObject/nInterfaceGlobals.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <signal.h> // linux for kill
const uint32_t solenoid_kNumDO7_0Elements = 8;
const uint32_t dio_kNumSystems = tDIO::kNumSystems;
const uint32_t interrupt_kNumSystems = tInterrupt::kNumSystems;
const uint32_t kSystemClockTicksPerMicrosecond = 40;

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
	tGlobal *global = tGlobal::create(status);
	uint16_t version = global->readVersion(status);
	delete global;
	return version;
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
	tGlobal *global = tGlobal::create(status);
	uint32_t revision = global->readRevision(status);
	delete global;
	return revision;
}

/**
 * Read the microsecond-resolution timer on the FPGA.
 * 
 * @return The current time in microseconds according to the FPGA (since FPGA reset).
 */
uint32_t getFPGATime(int32_t *status)
{
	tGlobal *global = tGlobal::create(status);
	uint32_t time = global->readLocalTime(status);
	delete global;
	return time;
}

/**
 * Set the state of the FPGA status LED on the cRIO.
 */
void setFPGALED(uint32_t state, int32_t *status)
{
	// XXX: Not supported?
	// tGlobal *global = tGlobal::create(status);
	// global->writeFPGA_LED(state, status);
	// delete global;
}

/**
 * Get the current state of the FPGA status LED on the cRIO.
 * @return The curent state of the FPGA LED.
 */
int32_t getFPGALED(int32_t *status)
{
	// XXX: Not supported?
	// tGlobal *global = tGlobal::create(status);
	// bool ledValue = global->readFPGA_LED(status);
	// delete global;
	// return ledValue;
	return 0; // XXX: Dummy value
}

int HALSetErrorData(const char *errors, int errorsLength, int wait_ms)
{
	return setErrorData(errors, errorsLength, wait_ms);
}

int HALSetUserDsLcdData(const char *userDsLcdData, int userDsLcdDataLength,
		int wait_ms)
{
	return setUserDsLcdData(userDsLcdData, userDsLcdDataLength, wait_ms);
}

int HALOverrideIOConfig(const char *ioConfig, int wait_ms)
{
	return overrideIOConfig(ioConfig, wait_ms);
}

int HALGetDynamicControlData(uint8_t type, char *dynamicData, int32_t maxLength,
		int wait_ms)
{
	return getDynamicControlData(type, dynamicData, maxLength, wait_ms);
}

int HALGetCommonControlData(HALCommonControlData *data, int wait_ms)
{
	return getRecentCommonControlData((FRCCommonControlData*) data, wait_ms);
}

void HALSetNewDataSem(pthread_mutex_t * param)
{
	setNewDataSem(param);
}

int HALSetStatusData(float battery, uint8_t dsDigitalOut, uint8_t updateNumber,
		const char *userDataHigh, int userDataHighLength,
		const char *userDataLow, int userDataLowLength, int wait_ms)
{
	return setStatusData(battery, dsDigitalOut, updateNumber, userDataHigh,
			userDataHighLength, userDataLow, userDataLowLength, wait_ms);
}

/**
 * Call this to start up HAL. This is required for robot programs.
 */
int HALInitialize(int mode)
{
	// image 4; Fixes errors caused by multiple processes. Talk to NI about this
	nFPGA::nRoboRIO_FPGANamespace::g_currentTargetClass =
			nLoadOut::kTargetClass_RoboRIO;

	// Kill any previous robot programs
	std::fstream fs;
	fs.open("/var/lock/frc.pid", std::fstream::in | std::fstream::out); // By making this both in/out, it won't give us an error if it doesnt exist
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
	//return FRC_NetworkCommunication_nUsageReporting_report( resource, instanceNumber, context, feature);
	return 0;
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

#include "NetworkCommunication/LoadOut.h"
namespace nLoadOut
{
bool getModulePresence(tModuleType moduleType, uint8_t moduleNumber)
{
	return true;
}
}
