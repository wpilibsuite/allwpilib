// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/HAL.h"

#include <dlfcn.h>
#include <signal.h>  // linux for kill
#include <sys/prctl.h>
#include <unistd.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <thread>
#include <utility>

#include <wpi/MemoryBuffer.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>
#include <wpi/mutex.h>
#include <wpi/print.h>
#include <wpi/timestamp.h>

#include "CANInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "SystemServerInternal.h"
#include "hal/DriverStation.h"
#include "hal/Errors.h"
#include "hal/Notifier.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

static uint64_t dsStartTime;

static int32_t teamNumber = -1;

using namespace hal;

namespace hal {
void InitializeDriverStation();
void WaitForInitialPacket();
namespace init {
void InitializeHAL() {
  InitializeCTREPCM();
  InitializeREVPH();
  InitializeAddressableLED();
  InitializeAnalogInput();
  InitializeAnalogTrigger();
  InitializeCAN();
  InitializeCANAPI();
  InitializeConstants();
  InitializeCounter();
  InitializeDIO();
  InitializeDutyCycle();
  InitializeEncoder();
  InitializeFRCDriverStation();
  InitializeI2C();
  InitializeLEDs();
  InitializeMain();
  InitializeNotifier();
  InitializeCTREPDP();
  InitializeREVPDH();
  InitializePorts();
  InitializePower();
  InitializePWM();
  InitializeSerialPort();
  InitializeSmartIo();
  InitializeThreads();
}
}  // namespace init

uint64_t GetDSInitializeTime() {
  return dsStartTime;
}

}  // namespace hal

extern "C" {

const char* HAL_GetErrorMessage(int32_t code) {
  switch (code) {
    case 0:
      return "";
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
    case HAL_CAN_BUFFER_OVERRUN:
      return HAL_CAN_BUFFER_OVERRUN_MESSAGE;
    case HAL_LED_CHANNEL_ERROR:
      return HAL_LED_CHANNEL_ERROR_MESSAGE;
    case HAL_INVALID_DMA_STATE:
      return HAL_INVALID_DMA_STATE_MESSAGE;
    case HAL_INVALID_DMA_ADDITION:
      return HAL_INVALID_DMA_ADDITION_MESSAGE;
    case HAL_USE_LAST_ERROR:
      return HAL_USE_LAST_ERROR_MESSAGE;
    case HAL_CONSOLE_OUT_ENABLED_ERROR:
      return HAL_CONSOLE_OUT_ENABLED_ERROR_MESSAGE;
    default:
      return "Unknown error status";
  }
}

static HAL_RuntimeType runtimeType = HAL_Runtime_SystemCore;

HAL_RuntimeType HAL_GetRuntimeType(void) {
  return runtimeType;
}

int32_t HAL_GetFPGAVersion(int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int64_t HAL_GetFPGARevision(int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_GetSerialNumber(struct WPI_String* serialNumber) {
  const char* serialNum = std::getenv("serialnum");
  if (!serialNum) {
    serialNum = "";
  }
  size_t len = std::strlen(serialNum);
  auto write = WPI_AllocateString(serialNumber, len);
  std::memcpy(write, serialNum, len);
}

void HAL_GetComments(struct WPI_String* comments) {
  comments->len = 0;
  comments->str = nullptr;
}

void InitializeTeamNumber(void) {
  char hostnameBuf[25];
  auto status = gethostname(hostnameBuf, sizeof(hostnameBuf));
  if (status != 0) {
    teamNumber = 0;
    return;
  }

  std::string_view hostname{hostnameBuf, sizeof(hostnameBuf)};

  // hostname is frc-{TEAM}-roborio
  // Split string around '-' (max of 2 splits), take the second element
  teamNumber = 0;
  int i = 0;
  wpi::split(hostname, '-', 2, false, [&](auto part) {
    if (i == 1) {
      teamNumber = wpi::parse_integer<int32_t>(part, 10).value_or(0);
    }
    ++i;
  });
}

int32_t HAL_GetTeamNumber(void) {
  if (teamNumber == -1) {
    InitializeTeamNumber();
  }
  return teamNumber;
}

uint64_t HAL_GetFPGATime(int32_t* status) {
  hal::init::CheckInit();
  return wpi::NowDefault();
}

uint64_t HAL_ExpandFPGATime(uint32_t unexpandedLower, int32_t* status) {
  // Capture the current FPGA time.  This will give us the upper half of the
  // clock.
  uint64_t fpgaTime = HAL_GetFPGATime(status);
  if (*status != 0) {
    return 0;
  }

  // Now, we need to detect the case where the lower bits rolled over after we
  // sampled.  In that case, the upper bits will be 1 bigger than they should
  // be.

  // Break it into lower and upper portions.
  uint32_t lower = fpgaTime & 0xffffffffull;
  uint64_t upper = (fpgaTime >> 32) & 0xffffffff;

  // The time was sampled *before* the current time, so roll it back.
  if (lower < unexpandedLower) {
    --upper;
  }

  return (upper << 32) + static_cast<uint64_t>(unexpandedLower);
}

HAL_Bool HAL_GetFPGAButton(int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_GetSystemActive(int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_GetBrownedOut(int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return false;
}

int32_t HAL_GetCommsDisableCount(int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return 0;
}

HAL_Bool HAL_GetRSLState(int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_GetSystemTimeValid(int32_t* status) {
  return false;
}

HAL_Bool HAL_Initialize(int32_t timeout, int32_t mode) {
  static std::atomic_bool initialized{false};
  static wpi::mutex initializeMutex;
  // Initial check, as if it's true initialization has finished
  if (initialized) {
    return true;
  }

  std::scoped_lock lock(initializeMutex);
  // Second check in case another thread was waiting
  if (initialized) {
    return true;
  }

  hal::init::InitializeHAL();

  hal::init::HAL_IsInitialized.store(true);

  setlinebuf(stdin);
  setlinebuf(stdout);

  prctl(PR_SET_PDEATHSIG, SIGTERM);

  if (!hal::InitializeCanBuses()) {
    std::printf("Failed to initialize can buses\n");
    return false;
  }

  hal::InitializeSystemServer();

  // // Return false if program failed to kill an existing program
  // if (!killExistingProgram(timeout, mode)) {
  //   return false;
  // }

  // FRC_NetworkCommunication_Reserve(nullptr);

  int32_t status = 0;

  hal::InitializeDriverStation();

  dsStartTime = HAL_GetFPGATime(&status);
  if (status != 0) {
    return false;
  }

  hal::WaitForInitialPacket();

  initialized = true;
  return true;
}

void HAL_Shutdown(void) {}

void HAL_SimPeriodicBefore(void) {}

void HAL_SimPeriodicAfter(void) {}

int64_t HAL_Report(int32_t resource, int32_t instanceNumber, int32_t context,
                   const char* feature) {
  if (feature == nullptr) {
    feature = "";
  }

  return 0;

  // return FRC_NetworkCommunication_nUsageReporting_report(
  //     resource, instanceNumber, context, feature);
}

}  // extern "C"
