/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/HAL.h"

#include <signal.h>  // linux for kill
#include <sys/prctl.h>
#include <unistd.h>

#include <atomic>
#include <cstdlib>
#include <fstream>
#include <thread>

#include <FRC_NetworkCommunication/FRCComm.h>
#include <FRC_NetworkCommunication/LoadOut.h>
#include <FRC_NetworkCommunication/UsageReporting.h>
#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "HALInitializer.h"
#include "ctre/ctre.h"
#include "hal/ChipObject.h"
#include "hal/DriverStation.h"
#include "hal/Errors.h"
#include "hal/Notifier.h"
#include "hal/handles/HandlesInternal.h"
#include "visa/visa.h"

using namespace hal;

static std::unique_ptr<tGlobal> global;
static std::unique_ptr<tSysWatchdog> watchdog;

using namespace hal;

namespace hal {
namespace init {
void InitializeHAL() {
  InitializeAccelerometer();
  InitializeAnalogAccumulator();
  InitializeAnalogGyro();
  InitializeAnalogInput();
  InitializeAnalogInternal();
  InitializeAnalogOutput();
  InitializeAnalogTrigger();
  InitializeCAN();
  InitializeCANAPI();
  InitializeCompressor();
  InitializeConstants();
  InitializeCounter();
  InitializeDigitalInternal();
  InitializeDIO();
  InitializeEncoder();
  InitializeFPGAEncoder();
  InitializeFRCDriverStation();
  InitializeI2C();
  InitialzeInterrupts();
  InitializeNotifier();
  InitializePCMInternal();
  InitializePDP();
  InitializePorts();
  InitializePower();
  InitializePWM();
  InitializeRelay();
  InitializeSerialPort();
  InitializeSolenoid();
  InitializeSPI();
  InitializeThreads();
}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_PortHandle HAL_GetPort(int32_t channel) {
  // Dont allow a number that wouldn't fit in a uint8_t
  if (channel < 0 || channel >= 255) return HAL_kInvalidHandle;
  return createPortHandle(channel, 1);
}

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
    case HAL_ERR_CANSessionMux_InvalidBuffer:
      return ERR_CANSessionMux_InvalidBuffer_MESSAGE;
    case HAL_ERR_CANSessionMux_MessageNotFound:
      return ERR_CANSessionMux_MessageNotFound_MESSAGE;
    case HAL_WARN_CANSessionMux_NoToken:
      return WARN_CANSessionMux_NoToken_MESSAGE;
    case HAL_ERR_CANSessionMux_NotAllowed:
      return ERR_CANSessionMux_NotAllowed_MESSAGE;
    case HAL_ERR_CANSessionMux_NotInitialized:
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
    case HAL_CAN_TIMEOUT:
      return HAL_CAN_TIMEOUT_MESSAGE;
    case ERR_FRCSystem_NetCommNotResponding:
      return ERR_FRCSystem_NetCommNotResponding_MESSAGE;
    case ERR_FRCSystem_NoDSConnection:
      return ERR_FRCSystem_NoDSConnection_MESSAGE;
    default:
      return "Unknown error status";
  }
}

HAL_RuntimeType HAL_GetRuntimeType(void) { return HAL_Athena; }

int32_t HAL_GetFPGAVersion(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  return global->readVersion(status);
}

int64_t HAL_GetFPGARevision(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  return global->readRevision(status);
}

uint64_t HAL_GetFPGATime(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  *status = 0;
  uint64_t upper1 = global->readLocalTimeUpper(status);
  uint32_t lower = global->readLocalTime(status);
  uint64_t upper2 = global->readLocalTimeUpper(status);
  if (*status != 0) return 0;
  if (upper1 != upper2) {
    // Rolled over between the lower call, reread lower
    lower = global->readLocalTime(status);
    if (*status != 0) return 0;
  }
  return (upper2 << 32) + lower;
}

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

void HAL_BaseInitialize(int32_t* status) {
  static std::atomic_bool initialized{false};
  static wpi::mutex initializeMutex;
  // Initial check, as if it's true initialization has finished
  if (initialized) return;

  std::lock_guard<wpi::mutex> lock(initializeMutex);
  // Second check in case another thread was waiting
  if (initialized) return;
  // image 4; Fixes errors caused by multiple processes. Talk to NI about this
  nFPGA::nRoboRIO_FPGANamespace::g_currentTargetClass =
      nLoadOut::kTargetClass_RoboRIO;

  global.reset(tGlobal::create(status));
  watchdog.reset(tSysWatchdog::create(status));
  initialized = true;
}

static bool killExistingProgram(int timeout, int mode) {
  // Kill any previous robot programs
  std::fstream fs;
  // By making this both in/out, it won't give us an error if it doesnt exist
  fs.open("/var/lock/frc.pid", std::fstream::in | std::fstream::out);
  if (fs.bad()) return false;

  pid_t pid = 0;
  if (!fs.eof() && !fs.fail()) {
    fs >> pid;
    // see if the pid is around, but we don't want to mess with init id=1, or
    // ourselves
    if (pid >= 2 && kill(pid, 0) == 0 && pid != getpid()) {
      wpi::outs() << "Killing previously running FRC program...\n";
      kill(pid, SIGTERM);  // try to kill it
      std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
      if (kill(pid, 0) == 0) {
        // still not successfull
        wpi::outs() << "FRC pid " << pid << " did not die within " << timeout
                    << "ms. Force killing with kill -9\n";
        // Force kill -9
        auto forceKill = kill(pid, SIGKILL);
        if (forceKill != 0) {
          auto errorMsg = std::strerror(forceKill);
          wpi::outs() << "Kill -9 error: " << errorMsg << "\n";
        }
        // Give a bit of time for the kill to take place
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
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
  return true;
}

HAL_Bool HAL_Initialize(int32_t timeout, int32_t mode) {
  static std::atomic_bool initialized{false};
  static wpi::mutex initializeMutex;
  // Initial check, as if it's true initialization has finished
  if (initialized) return true;

  std::lock_guard<wpi::mutex> lock(initializeMutex);
  // Second check in case another thread was waiting
  if (initialized) return true;

  hal::init::InitializeHAL();

  hal::init::HAL_IsInitialized.store(true);

  setlinebuf(stdin);
  setlinebuf(stdout);
  wpi::outs().SetUnbuffered();

  prctl(PR_SET_PDEATHSIG, SIGTERM);

  // Return false if program failed to kill an existing program
  if (!killExistingProgram(timeout, mode)) {
    return false;
  }

  FRC_NetworkCommunication_Reserve(nullptr);

  std::atexit([]() {
    // Unregister our new data condition variable.
    setNewDataSem(nullptr);
  });

  int32_t status = 0;
  HAL_BaseInitialize(&status);
  if (status != 0) return false;

  HAL_InitializeDriverStation();

  // Set WPI_Now to use FPGA timestamp
  wpi::SetNowImpl([]() -> uint64_t {
    int32_t status = 0;
    uint64_t rv = HAL_GetFPGATime(&status);
    if (status != 0) {
      wpi::errs()
          << "Call to HAL_GetFPGATime failed."
          << "Initialization might have failed. Time will not be correct\n";
      wpi::errs().flush();
      return 0u;
    }
    return rv;
  });

  initialized = true;
  return true;
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
void NumericArrayResize(void) {}
void RTSetCleanupProc(void) {}
void EDVR_CreateReference(void) {}

}  // extern "C"
