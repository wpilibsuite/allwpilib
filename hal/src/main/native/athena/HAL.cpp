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

#include <FRC_NetworkCommunication/FRCComm.h>
#include <FRC_NetworkCommunication/LoadOut.h>
#include <FRC_NetworkCommunication/UsageReporting.h>
#include <wpi/MemoryBuffer.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>
#include <wpi/mutex.h>
#include <wpi/print.h>
#include <wpi/timestamp.h>

#include "FPGACalls.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "hal/ChipObject.h"
#include "hal/DriverStation.h"
#include "hal/Errors.h"
#include "hal/Notifier.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/roborio/HMB.h"
#include "hal/roborio/InterruptManager.h"
#include "visa/visa.h"

using namespace hal;

static std::unique_ptr<tGlobal> global;
static std::unique_ptr<tSysWatchdog> watchdog;
static uint64_t dsStartTime;

static char roboRioCommentsString[64];
static size_t roboRioCommentsStringSize;
static bool roboRioCommentsStringInitialized;

static int32_t teamNumber = -1;

static const volatile HAL_HMBData* hmbBuffer;
#define HAL_HMB_TIMESTAMP_OFFSET 5

using namespace hal;

namespace hal {
void InitializeDriverStation();
void WaitForInitialPacket();
namespace init {
void InitializeHAL() {
  InitializeCTREPCM();
  InitializeREVPH();
  InitializeAddressableLED();
  InitializeAccelerometer();
  InitializeAnalogAccumulator();
  InitializeAnalogGyro();
  InitializeAnalogInput();
  InitializeAnalogInternal();
  InitializeAnalogOutput();
  InitializeAnalogTrigger();
  InitializeCAN();
  InitializeCANAPI();
  InitializeCAN2027API();
  InitializeConstants();
  InitializeCounter();
  InitializeDigitalInternal();
  InitializeDIO();
  InitializeDMA();
  InitializeDutyCycle();
  InitializeEncoder();
  InitializeFPGAEncoder();
  InitializeFRCDriverStation();
  InitializeI2C();
  InitializeInterrupts();
  InitializeLEDs();
  InitializeMain();
  InitializeNotifier();
  InitializeCTREPDP();
  InitializeREVPDH();
  InitializePorts();
  InitializePower();
  InitializePWM();
  InitializeRelay();
  InitializeSerialPort();
  InitializeSPI();
  InitializeThreads();
}
}  // namespace init

void ReleaseFPGAInterrupt(int32_t interruptNumber) {
  hal::init::CheckInit();
  if (!global) {
    return;
  }
  int32_t status = 0;
  global->writeInterruptForceNumber(static_cast<unsigned char>(interruptNumber),
                                    &status);
  global->strobeInterruptForceOnce(&status);
}

uint64_t GetDSInitializeTime() {
  return dsStartTime;
}

}  // namespace hal

extern "C" {

HAL_PortHandle HAL_GetPort(int32_t channel) {
  // Dont allow a number that wouldn't fit in a uint8_t
  if (channel < 0 || channel >= 255) {
    return HAL_kInvalidHandle;
  }
  return createPortHandle(channel, 1);
}

HAL_PortHandle HAL_GetPortWithModule(int32_t module, int32_t channel) {
  // Dont allow a number that wouldn't fit in a uint8_t
  if (channel < 0 || channel >= 255) {
    return HAL_kInvalidHandle;
  }
  if (module < 0 || module >= 255) {
    return HAL_kInvalidHandle;
  }
  return createPortHandle(channel, module);
}

const char* HAL_GetErrorMessage(int32_t code) {
  switch (code) {
    case 0:
      return "";
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

static HAL_RuntimeType runtimeType = HAL_Runtime_RoboRIO;

HAL_RuntimeType HAL_GetRuntimeType(void) {
  return runtimeType;
}

int32_t HAL_GetFPGAVersion(int32_t* status) {
  hal::init::CheckInit();
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  return global->readVersion(status);
}

int64_t HAL_GetFPGARevision(int32_t* status) {
  hal::init::CheckInit();
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  return global->readRevision(status);
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

void InitializeRoboRioComments(void) {
  if (!roboRioCommentsStringInitialized) {
    auto fileBuffer = wpi::MemoryBuffer::GetFile("/etc/machine-info");
    if (!fileBuffer) {
      roboRioCommentsStringSize = 0;
      roboRioCommentsStringInitialized = true;
      return;
    }
    std::string_view fileContents{
        reinterpret_cast<const char*>(fileBuffer.value()->begin()),
        fileBuffer.value()->size()};
    std::string_view searchString = "PRETTY_HOSTNAME=\"";

    size_t start = fileContents.find(searchString);
    if (start == std::string_view::npos) {
      roboRioCommentsStringSize = 0;
      roboRioCommentsStringInitialized = true;
      return;
    }
    start += searchString.size();
    std::string_view escapedComments =
        wpi::slice(fileContents, start, fileContents.size());
    wpi::SmallString<64> buf;
    auto [unescapedComments, rem] = wpi::UnescapeCString(escapedComments, buf);
    unescapedComments.copy(roboRioCommentsString,
                           sizeof(roboRioCommentsString));

    if (unescapedComments.size() > sizeof(roboRioCommentsString)) {
      roboRioCommentsStringSize = sizeof(roboRioCommentsString);
    } else {
      roboRioCommentsStringSize = unescapedComments.size();
    }
    roboRioCommentsStringInitialized = true;
  }
}

void HAL_GetComments(struct WPI_String* comments) {
  if (!roboRioCommentsStringInitialized) {
    InitializeRoboRioComments();
  }
  auto write = WPI_AllocateString(comments, roboRioCommentsStringSize);
  std::memcpy(write, roboRioCommentsString, roboRioCommentsStringSize);
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
  // Split string around '-' (max of 2 splits), take the second element of the
  // resulting array.
  wpi::SmallVector<std::string_view> elements;
  wpi::split(hostname, elements, "-", 2);
  if (elements.size() < 3) {
    teamNumber = 0;
    return;
  }

  teamNumber = wpi::parse_integer<int32_t>(elements[1], 10).value_or(0);
}

int32_t HAL_GetTeamNumber(void) {
  if (teamNumber == -1) {
    InitializeTeamNumber();
  }
  return teamNumber;
}

uint64_t HAL_GetFPGATime(int32_t* status) {
  hal::init::CheckInit();
  if (!hmbBuffer) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }

  asm("dmb");
  uint64_t upper1 = hmbBuffer->Timestamp.Upper;
  asm("dmb");
  uint32_t lower = hmbBuffer->Timestamp.Lower;
  asm("dmb");
  uint64_t upper2 = hmbBuffer->Timestamp.Upper;

  if (upper1 != upper2) {
    // Rolled over between the lower call, reread lower
    asm("dmb");
    lower = hmbBuffer->Timestamp.Lower;
  }
  // 5 is added here because the time to write from the FPGA
  // to the HMB buffer is longer then the time to read
  // from the time register. This would cause register based
  // timestamps to be ahead of HMB timestamps, which could
  // be very bad.
  return (upper2 << 32) + lower + HAL_HMB_TIMESTAMP_OFFSET;
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
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return global->readUserButton(status);
}

HAL_Bool HAL_GetSystemActive(int32_t* status) {
  hal::init::CheckInit();
  if (!watchdog) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return watchdog->readStatus_SystemActive(status);
}

HAL_Bool HAL_GetBrownedOut(int32_t* status) {
  hal::init::CheckInit();
  if (!watchdog) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return !(watchdog->readStatus_PowerAlive(status));
}

int32_t HAL_GetCommsDisableCount(int32_t* status) {
  hal::init::CheckInit();
  if (!watchdog) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return 0;
  }
  return watchdog->readStatus_SysDisableCount(status);
}

HAL_Bool HAL_GetRSLState(int32_t* status) {
  hal::init::CheckInit();
  if (!global) {
    *status = NiFpga_Status_ResourceNotInitialized;
    return false;
  }
  return global->readLEDs_RSL(status);
}

HAL_Bool HAL_GetSystemTimeValid(int32_t* status) {
  uint8_t timeWasSet = 0;
  *status = FRC_NetworkCommunication_getTimeWasSet(&timeWasSet);
  return timeWasSet != 0;
}

static bool killExistingProgram(int timeout, int mode) {
  // Kill any previous robot programs
  std::fstream fs;
  // By making this both in/out, it won't give us an error if it doesn't exist
  fs.open("/var/lock/frc.pid", std::fstream::in | std::fstream::out);
  if (fs.bad()) {
    return false;
  }

  pid_t pid = 0;
  if (!fs.eof() && !fs.fail()) {
    fs >> pid;
    // see if the pid is around, but we don't want to mess with init id=1, or
    // ourselves
    if (pid >= 2 && kill(pid, 0) == 0 && pid != getpid()) {
      std::puts("Killing previously running FRC program...");
      kill(pid, SIGTERM);  // try to kill it
      std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
      if (kill(pid, 0) == 0) {
        // still not successful
        wpi::print(
            "FRC pid {} did not die within {} ms. Force killing with kill -9\n",
            pid, timeout);
        // Force kill -9
        auto forceKill = kill(pid, SIGKILL);
        if (forceKill != 0) {
          auto errorMsg = std::strerror(forceKill);
          wpi::print("Kill -9 error: {}\n", errorMsg);
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

static bool SetupNowRio(void) {
  nFPGA::nRoboRIO_FPGANamespace::g_currentTargetClass =
      nLoadOut::getTargetClass();

  int32_t status = 0;

  Dl_info info;
  status = dladdr(reinterpret_cast<void*>(tHMB::create), &info);
  if (status == 0) {
    wpi::print(stderr, "Failed to call dladdr on chipobject {}\n", dlerror());
    return false;
  }

  void* chipObjectLibrary = dlopen(info.dli_fname, RTLD_LAZY);
  if (chipObjectLibrary == nullptr) {
    wpi::print(stderr, "Failed to call dlopen on chipobject {}\n", dlerror());
    return false;
  }

  std::unique_ptr<tHMB> hmb;
  hmb.reset(tHMB::create(&status));
  if (hmb == nullptr) {
    wpi::print(stderr, "Failed to open HMB on chipobject {}\n", status);
    dlclose(chipObjectLibrary);
    return false;
  }
  return wpi::impl::SetupNowRio(chipObjectLibrary, std::move(hmb));
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

  int fpgaInit = hal::init::InitializeFPGA();
  if (fpgaInit != HAL_SUCCESS) {
    return false;
  }

  hal::init::InitializeHAL();

  hal::init::HAL_IsInitialized.store(true);

  setlinebuf(stdin);
  setlinebuf(stdout);

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

  if (!SetupNowRio()) {
    wpi::print(stderr,
               "Failed to run SetupNowRio(). This is a fatal error. The "
               "process is being terminated.\n");
    std::fflush(stderr);
    // Attempt to force a segfault to get a better java log
    *reinterpret_cast<int*>(0) = 0;
    // If that fails, terminate
    std::terminate();
    return false;
  }

  int32_t status = 0;

  HAL_InitializeHMB(&status);
  if (status != 0) {
    wpi::print(stderr, "Failed to open HAL HMB, status code {}\n", status);
    return false;
  }
  hmbBuffer = HAL_GetHMBBuffer();

  global.reset(tGlobal::create(&status));
  watchdog.reset(tSysWatchdog::create(&status));

  if (status != 0) {
    return false;
  }

  nLoadOut::tTargetClass targetClass = nLoadOut::getTargetClass();
  if (targetClass == nLoadOut::kTargetClass_RoboRIO2) {
    runtimeType = HAL_Runtime_RoboRIO2;
  } else {
    runtimeType = HAL_Runtime_RoboRIO;
  }

  InterruptManager::Initialize(global->getSystemInterface());

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

  return FRC_NetworkCommunication_nUsageReporting_report(
      resource, instanceNumber, context, feature);
}

}  // extern "C"
