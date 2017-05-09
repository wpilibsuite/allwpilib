/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"

#include <signal.h>  // linux for kill
#include <sys/prctl.h>
#include <unistd.h>

#include <atomic>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

#include "FRC_NetworkCommunication/CANSessionMux.h"
#include "FRC_NetworkCommunication/FRCComm.h"
#include "FRC_NetworkCommunication/LoadOut.h"
#include "HAL/ChipObject.h"
#include "HAL/DriverStation.h"
#include "HAL/Errors.h"
#include "HAL/Notifier.h"
#include "HAL/cpp/priority_mutex.h"
#include "HAL/handles/HandlesInternal.h"
#include "ctre/ctre.h"
#include "visa/visa.h"

using namespace hal;

static std::unique_ptr<tGlobal> global;
static std::unique_ptr<tSysWatchdog> watchdog;

static hal::priority_mutex timeMutex;
static uint32_t timeEpoch = 0;
static uint32_t prevFPGATime = 0;
static HAL_NotifierHandle rolloverNotifier = 0;

using namespace hal;

extern "C" {

HAL_PortHandle HAL_GetPort(int32_t channel) {
  // Dont allow a number that wouldn't fit in a uint8_t
  if (channel < 0 || channel >= 255) return HAL_kInvalidHandle;
  return createPortHandle(channel, 1);
}

/**
 * @deprecated Uses module numbers
 */
HAL_PortHandle HAL_GetPortWithModule(int32_t module, int32_t channel) {
  // Dont allow a number that wouldn't fit in a uint8_t
  if (channel < 0 || channel >= 255) return HAL_kInvalidHandle;
  if (module < 0 || module >= 255) return HAL_kInvalidHandle;
  return createPortHandle(channel, module);
}

const char* HAL_GetErrorMessage(int32_t code) {
  switch (code) {
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
    case RESOURCE_IS_ALLOCATED:
      return RESOURCE_IS_ALLOCATED_MESSAGE;
    case RESOURCE_OUT_OF_RANGE:
      return RESOURCE_OUT_OF_RANGE_MESSAGE;
    case HAL_INVALID_ACCUMULATOR_CHANNEL:
      return HAL_INVALID_ACCUMULATOR_CHANNEL_MESSAGE;
    case HAL_HANDLE_ERROR:
      return HAL_HANDLE_ERROR_MESSAGE;
    case NULL_PARAMETER:
      return NULL_PARAMETER_MESSAGE;
    case ANALOG_TRIGGER_LIMIT_ORDER_ERROR:
      return ANALOG_TRIGGER_LIMIT_ORDER_ERROR_MESSAGE;
    case ANALOG_TRIGGER_PULSE_OUTPUT_ERROR:
      return ANALOG_TRIGGER_PULSE_OUTPUT_ERROR_MESSAGE;
    case PARAMETER_OUT_OF_RANGE:
      return PARAMETER_OUT_OF_RANGE_MESSAGE;
    case HAL_COUNTER_NOT_SUPPORTED:
      return HAL_COUNTER_NOT_SUPPORTED_MESSAGE;
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
    case HAL_PWM_SCALE_ERROR:
      return HAL_PWM_SCALE_ERROR_MESSAGE;
    case HAL_SERIAL_PORT_NOT_FOUND:
      return HAL_SERIAL_PORT_NOT_FOUND_MESSAGE;
    case HAL_THREAD_PRIORITY_ERROR:
      return HAL_THREAD_PRIORITY_ERROR_MESSAGE;
    case HAL_THREAD_PRIORITY_RANGE_ERROR:
      return HAL_THREAD_PRIORITY_RANGE_ERROR_MESSAGE;
    case HAL_SERIAL_PORT_OPEN_ERROR:
      return HAL_SERIAL_PORT_OPEN_ERROR_MESSAGE;
    case HAL_SERIAL_PORT_ERROR:
      return HAL_SERIAL_PORT_ERROR_MESSAGE;
    default:
      return "Unknown error status";
  }
}

/**
 * Returns the runtime type of this HAL
 */
HAL_RuntimeType HAL_GetRuntimeType() { return HAL_Athena; }

/**
 * Return the FPGA Version number.
 * For now, expect this to be competition year.
 * @return FPGA Version number.
 */
int32_t HAL_GetFPGAVersion(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
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
int64_t HAL_GetFPGARevision(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  return global->readRevision(status);
}

/**
 * Read the microsecond-resolution timer on the FPGA.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA
 * reset).
 */
uint64_t HAL_GetFPGATime(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  std::lock_guard<hal::priority_mutex> lock(timeMutex);
  uint32_t fpgaTime = global->readLocalTime(status);
  if (*status != 0) return 0;
  // check for rollover
  if (fpgaTime < prevFPGATime) ++timeEpoch;
  prevFPGATime = fpgaTime;
  return static_cast<uint64_t>(timeEpoch) << 32 |
         static_cast<uint64_t>(fpgaTime);
}

/**
 * Get the state of the "USER" button on the roboRIO
 * @return true if the button is currently pressed down
 */
HAL_Bool HAL_GetFPGAButton(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return global->readUserButton(status);
}

HAL_Bool HAL_GetSystemActive(int32_t* status) {
  if (!watchdog) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return watchdog->readStatus_SystemActive(status);
}

HAL_Bool HAL_GetBrownedOut(int32_t* status) {
  if (!watchdog) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return !(watchdog->readStatus_PowerAlive(status));
}

static void timerRollover(uint64_t currentTime, HAL_NotifierHandle handle) {
  // reschedule timer for next rollover
  int32_t status = 0;
  HAL_UpdateNotifierAlarm(handle, currentTime + 0x80000000ULL, &status);
}

void HAL_BaseInitialize(int32_t* status) {
  static std::atomic_bool initialized{false};
  static hal::priority_mutex initializeMutex;
  // Initial check, as if it's true initialization has finished
  if (initialized) return;

  std::lock_guard<hal::priority_mutex> lock(initializeMutex);
  // Second check in case another thread was waiting
  if (initialized) return;
  // image 4; Fixes errors caused by multiple processes. Talk to NI about this
  nFPGA::nRoboRIO_FPGANamespace::g_currentTargetClass =
      nLoadOut::kTargetClass_RoboRIO;

  global.reset(tGlobal::create(status));
  watchdog.reset(tSysWatchdog::create(status));
  initialized = true;
}

/**
 * Call this to start up HAL. This is required for robot programs.
 */
int32_t HAL_Initialize(int32_t mode) {
  setlinebuf(stdin);
  setlinebuf(stdout);

  prctl(PR_SET_PDEATHSIG, SIGTERM);

  FRC_NetworkCommunication_Reserve(nullptr);

  std::atexit([]() {
    // Unregister our new data condition variable.
    setNewDataSem(nullptr);
  });

  int32_t status = 0;
  HAL_BaseInitialize(&status);

  if (!rolloverNotifier)
    rolloverNotifier = HAL_InitializeNotifier(timerRollover, nullptr, &status);
  if (status == 0) {
    uint64_t curTime = HAL_GetFPGATime(&status);
    if (status == 0)
      HAL_UpdateNotifierAlarm(rolloverNotifier, curTime + 0x80000000ULL,
                              &status);
  }

  // Kill any previous robot programs
  std::fstream fs;
  // By making this both in/out, it won't give us an error if it doesnt exist
  fs.open("/var/lock/frc.pid", std::fstream::in | std::fstream::out);
  if (fs.bad()) return 0;

  pid_t pid = 0;
  if (!fs.eof() && !fs.fail()) {
    fs >> pid;
    // see if the pid is around, but we don't want to mess with init id=1, or
    // ourselves
    if (pid >= 2 && kill(pid, 0) == 0 && pid != getpid()) {
      std::cout << "Killing previously running FRC program..." << std::endl;
      kill(pid, SIGTERM);  // try to kill it
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if (kill(pid, 0) == 0) {
        // still not successfull
        if (mode == 0) {
          std::cout << "FRC pid " << pid
                    << " did not die within 110ms. Aborting" << std::endl;
          return 0;              // just fail
        } else if (mode == 1) {  // kill -9 it
          kill(pid, SIGKILL);
        } else {
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

  HAL_InitializeDriverStation();

  return 1;
}

int64_t HAL_Report(int32_t resource, int32_t instanceNumber, int32_t context,
                   const char* feature) {
  if (feature == nullptr) {
    feature = "";
  }

  return FRC_NetworkCommunication_nUsageReporting_report(
      resource, instanceNumber, context, feature);
}

// TODO: HACKS
// No need for header definitions, as we should not run from user code.
void NumericArrayResize() {}
void RTSetCleanupProc() {}
void EDVR_CreateReference() {}

}  // extern "C"
