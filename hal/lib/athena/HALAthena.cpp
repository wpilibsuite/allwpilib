/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"
#include "HAL/cpp/priority_mutex.h"

#include <signal.h>  // linux for kill
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

#include "ChipObject.h"
#include "FRC_NetworkCommunication/CANSessionMux.h"
#include "FRC_NetworkCommunication/FRCComm.h"
#include "FRC_NetworkCommunication/LoadOut.h"
#include "HAL/Errors.h"
#include "ctre/ctre.h"
#include "handles/HandlesInternal.h"
#include "visa/visa.h"

const uint32_t solenoid_kNumDO7_0Elements = 8;
const uint32_t dio_kNumSystems = tDIO::kNumSystems;
const uint32_t interrupt_kNumSystems = tInterrupt::kNumSystems;
const uint32_t kSystemClockTicksPerMicrosecond = 40;

static tGlobal* global = nullptr;
static tSysWatchdog* watchdog = nullptr;

static priority_mutex timeMutex;
static priority_mutex msgMutex;
static uint32_t timeEpoch = 0;
static uint32_t prevFPGATime = 0;
static HalNotifierHandle rolloverNotifier = 0;

using namespace hal;

extern "C" {

HalPortHandle getPort(uint8_t pin) { return createPortHandle(pin, 1); }

/**
 * @deprecated Uses module numbers
 */
HalPortHandle getPortWithModule(uint8_t module, uint8_t pin) {
  return createPortHandle(pin, module);
}

void freePort(HalPortHandle port_handle) {
  // noop
}

const char* getHALErrorMessage(int32_t code) {
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
uint16_t getFPGAVersion(int32_t* status) {
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
uint32_t getFPGARevision(int32_t* status) {
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
uint64_t getFPGATime(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  std::lock_guard<priority_mutex> lock(timeMutex);
  uint32_t fpgaTime = global->readLocalTime(status);
  if (*status != 0) return 0;
  // check for rollover
  if (fpgaTime < prevFPGATime) ++timeEpoch;
  prevFPGATime = fpgaTime;
  return (((uint64_t)timeEpoch) << 32) | ((uint64_t)fpgaTime);
}

/**
 * Get the state of the "USER" button on the roboRIO
 * @return true if the button is currently pressed down
 */
bool getFPGAButton(int32_t* status) {
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return global->readUserButton(status);
}

int HALSetErrorData(const char* errors, int errorsLength, int wait_ms) {
  return setErrorData(errors, errorsLength, wait_ms);
}

int HALSendError(int isError, int32_t errorCode, int isLVCode,
                 const char* details, const char* location,
                 const char* callStack, int printMsg) {
  // Avoid flooding console by keeping track of previous 5 error
  // messages and only printing again if they're longer than 1 second old.
  static constexpr int KEEP_MSGS = 5;
  std::lock_guard<priority_mutex> lock(msgMutex);
  static std::string prev_msg[KEEP_MSGS];
  static uint64_t prev_msg_time[KEEP_MSGS] = {0, 0, 0};

  int32_t status = 0;
  uint64_t curTime = getFPGATime(&status);
  int i;
  for (i = 0; i < KEEP_MSGS; ++i) {
    if (prev_msg[i] == details) break;
  }
  int retval = 0;
  if (i == KEEP_MSGS || (curTime - prev_msg_time[i]) >= 1000000) {
    retval = FRC_NetworkCommunication_sendError(isError, errorCode, isLVCode,
                                                details, location, callStack);
    if (printMsg) {
      if (location && location[0] != '\0') {
        fprintf(stderr, "%s at %s: ", isError ? "Error" : "Warning", location);
      }
      fprintf(stderr, "%s\n", details);
      if (callStack && callStack[0] != '\0') {
        fprintf(stderr, "%s\n", callStack);
      }
    }
    if (i == KEEP_MSGS) {
      // replace the oldest one
      i = 0;
      uint64_t first = prev_msg_time[0];
      for (int j = 1; j < KEEP_MSGS; ++j) {
        if (prev_msg_time[j] < first) {
          first = prev_msg_time[j];
          i = j;
        }
      }
      prev_msg[i] = details;
    }
    prev_msg_time[i] = curTime;
  }
  return retval;
}

bool HALGetSystemActive(int32_t* status) {
  if (!watchdog) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return watchdog->readStatus_SystemActive(status);
}

bool HALGetBrownedOut(int32_t* status) {
  if (!watchdog) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return !(watchdog->readStatus_PowerAlive(status));
}

static void HALCleanupAtExit() {
  global = nullptr;
  watchdog = nullptr;
}

static void timerRollover(uint64_t currentTime, void*) {
  // reschedule timer for next rollover
  int32_t status = 0;
  updateNotifierAlarm(rolloverNotifier, currentTime + 0x80000000ULL, &status);
}

/**
 * Call this to start up HAL. This is required for robot programs.
 */
int HALInitialize(int mode) {
  setlinebuf(stdin);
  setlinebuf(stdout);

  prctl(PR_SET_PDEATHSIG, SIGTERM);

  FRC_NetworkCommunication_Reserve(nullptr);
  // image 4; Fixes errors caused by multiple processes. Talk to NI about this
  nFPGA::nRoboRIO_FPGANamespace::g_currentTargetClass =
      nLoadOut::kTargetClass_RoboRIO;

  int32_t status = 0;
  global = tGlobal::create(&status);
  watchdog = tSysWatchdog::create(&status);

  std::atexit(HALCleanupAtExit);

  if (!rolloverNotifier)
    rolloverNotifier = initializeNotifier(timerRollover, nullptr, &status);
  if (status == 0) {
    uint64_t curTime = getFPGATime(&status);
    if (status == 0)
      updateNotifierAlarm(rolloverNotifier, curTime + 0x80000000ULL, &status);
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
          return 0;            // just fail
        } else if (mode == 1)  // kill -9 it
          kill(pid, SIGKILL);
        else {
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

uint32_t HALReport(uint8_t resource, uint8_t instanceNumber, uint8_t context,
                   const char* feature) {
  if (feature == nullptr) {
    feature = "";
  }

  return FRC_NetworkCommunication_nUsageReporting_report(
      resource, instanceNumber, context, feature);
}

// TODO: HACKS
void NumericArrayResize() {}
void RTSetCleanupProc() {}
void EDVR_CreateReference() {}
void Occur() {}

void imaqGetErrorText() {}
void imaqGetLastError() {}

}  // extern "C"
