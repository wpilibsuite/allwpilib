#include "HAL/HAL.hpp"

#include "Port.h"
#include "HAL/Errors.hpp"
#include "ctre/ctre.h"
#include "visa/visa.h"
#include "ChipObject.h"
#include "NetworkCommunication/FRCComm.h"
#include "NetworkCommunication/UsageReporting.h"
#include "NetworkCommunication/LoadOut.h"
#include "NetworkCommunication/CANSessionMux.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h> // linux for kill
const uint32_t solenoid_kNumDO7_0Elements = 8;
const uint32_t dio_kNumSystems = tDIO::kNumSystems;
const uint32_t interrupt_kNumSystems = tInterrupt::kNumSystems;
const uint32_t kSystemClockTicksPerMicrosecond = 40;

static tGlobal *global;
static tSysWatchdog *watchdog;

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
	switch(code) {
		case 0:
			return "";
		case CTR_RxTimeout:
			return CTR_RxTimeout_MESSAGE;
		case CTR_TxTimeout:
			return CTR_TxTimeout_MESSAGE;
		case CTR_InvalidParamValue:
			return CTR_InvalidParamValue_MESSAGE;
		case CTR_UnexpectedArbId:
			return CTR_UnexpectedArbId_MESSAGE;
                case CTR_TxFailed:
			return CTR_TxFailed_MESSAGE;
                case CTR_SigNotUpdated:
			return CTR_SigNotUpdated_MESSAGE;
		case NiFpga_Status_FifoTimeout:
			return NiFpga_Status_FifoTimeout_MESSAGE;
		case NiFpga_Status_TransferAborted:
			return NiFpga_Status_TransferAborted_MESSAGE;
		case NiFpga_Status_MemoryFull: 
			return NiFpga_Status_MemoryFull_MESSAGE;
		case NiFpga_Status_SoftwareFault:
			return NiFpga_Status_SoftwareFault_MESSAGE;
		case NiFpga_Status_InvalidParameter:
			return NiFpga_Status_InvalidParameter_MESSAGE;
		case NiFpga_Status_ResourceNotFound:
			return NiFpga_Status_ResourceNotFound_MESSAGE; 
		case NiFpga_Status_ResourceNotInitialized:
			return NiFpga_Status_ResourceNotInitialized_MESSAGE; 
		case NiFpga_Status_HardwareFault:
			return NiFpga_Status_HardwareFault_MESSAGE;
		case NiFpga_Status_IrqTimeout:
			return NiFpga_Status_IrqTimeout_MESSAGE;
		case SAMPLE_RATE_TOO_HIGH:
			return SAMPLE_RATE_TOO_HIGH_MESSAGE;
		case VOLTAGE_OUT_OF_RANGE:
			return VOLTAGE_OUT_OF_RANGE_MESSAGE;
		case LOOP_TIMING_ERROR:
			return LOOP_TIMING_ERROR_MESSAGE;
		case SPI_WRITE_NO_MOSI:
			return SPI_WRITE_NO_MOSI_MESSAGE;
		case SPI_READ_NO_MISO:
			return SPI_READ_NO_MISO_MESSAGE;
		case SPI_READ_NO_DATA:
			return SPI_READ_NO_DATA_MESSAGE;
		case INCOMPATIBLE_STATE:
			return INCOMPATIBLE_STATE_MESSAGE;
		case NO_AVAILABLE_RESOURCES:
			return NO_AVAILABLE_RESOURCES_MESSAGE;
		case NULL_PARAMETER:
			return NULL_PARAMETER_MESSAGE;
		case ANALOG_TRIGGER_LIMIT_ORDER_ERROR:
			return ANALOG_TRIGGER_LIMIT_ORDER_ERROR_MESSAGE;
		case ANALOG_TRIGGER_PULSE_OUTPUT_ERROR:
			return ANALOG_TRIGGER_PULSE_OUTPUT_ERROR_MESSAGE;
		case PARAMETER_OUT_OF_RANGE:
			return PARAMETER_OUT_OF_RANGE_MESSAGE;
		case ERR_CANSessionMux_InvalidBuffer:
			return ERR_CANSessionMux_InvalidBuffer_MESSAGE;
		case ERR_CANSessionMux_MessageNotFound:
			return ERR_CANSessionMux_MessageNotFound_MESSAGE;
		case WARN_CANSessionMux_NoToken:
			return WARN_CANSessionMux_NoToken_MESSAGE;
		case ERR_CANSessionMux_NotAllowed:
			return ERR_CANSessionMux_NotAllowed_MESSAGE;
		case ERR_CANSessionMux_NotInitialized:
			return ERR_CANSessionMux_NotInitialized_MESSAGE;
		case VI_ERROR_SYSTEM_ERROR:
			return VI_ERROR_SYSTEM_ERROR_MESSAGE;
		case VI_ERROR_INV_OBJECT:
			return VI_ERROR_INV_OBJECT_MESSAGE;
		case VI_ERROR_RSRC_LOCKED:
			return VI_ERROR_RSRC_LOCKED_MESSAGE;
		case VI_ERROR_RSRC_NFOUND:
			return VI_ERROR_RSRC_NFOUND_MESSAGE;
		case VI_ERROR_INV_RSRC_NAME:
			return VI_ERROR_INV_RSRC_NAME_MESSAGE;
		case VI_ERROR_QUEUE_OVERFLOW:
			return VI_ERROR_QUEUE_OVERFLOW_MESSAGE;
		case VI_ERROR_IO:
			return VI_ERROR_IO_MESSAGE;
		case VI_ERROR_ASRL_PARITY:
			return VI_ERROR_ASRL_PARITY_MESSAGE;
		case VI_ERROR_ASRL_FRAMING:
			return VI_ERROR_ASRL_FRAMING_MESSAGE;
		case VI_ERROR_ASRL_OVERRUN:
			return VI_ERROR_ASRL_OVERRUN_MESSAGE;
		case VI_ERROR_RSRC_BUSY:
			return VI_ERROR_RSRC_BUSY_MESSAGE;
		case VI_ERROR_INV_PARAMETER:
			return VI_ERROR_INV_PARAMETER_MESSAGE;
		default:
			return "Unknown error status";
	}
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

int HALGetJoystickAxes(uint8_t joystickNum, HALJoystickAxes *axes)
{
	return FRC_NetworkCommunication_getJoystickAxes(joystickNum, (JoystickAxes_t*) axes, kMaxJoystickAxes);
}

int HALGetJoystickPOVs(uint8_t joystickNum, HALJoystickPOVs *povs)
{
	return FRC_NetworkCommunication_getJoystickPOVs(joystickNum, (JoystickPOV_t*) povs, kMaxJoystickPOVs);
}

int HALGetJoystickButtons(uint8_t joystickNum, HALJoystickButtons *buttons)
{
	return FRC_NetworkCommunication_getJoystickButtons(joystickNum, &buttons->buttons, &buttons->count);
}

int HALGetJoystickDescriptor(uint8_t joystickNum, HALJoystickDescriptor *desc)
{
	return FRC_NetworkCommunication_getJoystickDesc(joystickNum, &desc->isXbox, &desc->type, (char *)(&desc->name), 
		&desc->axisCount, &desc->axisTypes, &desc->buttonCount, &desc->povCount);
}

int HALSetJoystickOutputs(uint8_t joystickNum, uint32_t outputs, uint16_t leftRumble, uint16_t rightRumble)
{
	return FRC_NetworkCommunication_setJoystickOutputs(joystickNum, outputs, leftRumble, rightRumble);
}

int HALGetMatchTime(float *matchTime)
{
	return FRC_NetworkCommunication_getMatchTime(matchTime);
}

void HALSetNewDataSem(MULTIWAIT_ID sem)
{
	setNewDataSem(sem);
}

bool HALGetSystemActive(int32_t *status)
{
	return watchdog->readStatus_SystemActive(status);
}
	
bool HALGetBrownedOut(int32_t *status)
{
	return !(watchdog->readStatus_PowerAlive(status));
}

/**
 * Call this to start up HAL. This is required for robot programs.
 */
int HALInitialize(int mode)
{
    setlinebuf(stdin);
    setlinebuf(stdout);

    prctl(PR_SET_PDEATHSIG, SIGTERM);

	FRC_NetworkCommunication_Reserve(nullptr);
	// image 4; Fixes errors caused by multiple processes. Talk to NI about this
	nFPGA::nRoboRIO_FPGANamespace::g_currentTargetClass =
			nLoadOut::kTargetClass_RoboRIO;

	int32_t status;
	global = tGlobal::create(&status);
	watchdog = tSysWatchdog::create(&status);

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
