// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/HAL.h"

#include <cstdio>
#include <cstring>
#include <utility>
#include <vector>

#include <wpi/mutex.h>
#include <wpi/spinlock.h>

#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "ntdll.lib")
extern "C" NTSYSAPI NTSTATUS NTAPI NtSetTimerResolution(
    ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);
extern "C" NTSYSAPI NTSTATUS NTAPI
NtQueryTimerResolution(PULONG MinimumResolution, PULONG MaximumResolution,
                       PULONG CurrentResolution);
#endif  // _WIN32

#include "ErrorsInternal.h"
#include "HALInitializer.h"
#include "MockHooksInternal.h"
#include "hal/Errors.h"
#include "hal/Extensions.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/simulation/DriverStationData.h"
#include "hal/simulation/SimCallbackRegistry.h"
#include "mockdata/RoboRioDataInternal.h"

using namespace hal;

namespace {
class SimPeriodicCallbackRegistry : public impl::SimCallbackRegistryBase {
 public:
  int32_t Register(HALSIM_SimPeriodicCallback callback, void* param) {
    std::scoped_lock lock(m_mutex);
    return DoRegister(reinterpret_cast<RawFunctor>(callback), param);
  }

  void operator()() const {
    std::scoped_lock lock(m_mutex);
    if (m_callbacks) {
      for (auto&& cb : *m_callbacks) {
        reinterpret_cast<HALSIM_SimPeriodicCallback>(cb.callback)(cb.param);
      }
    }
  }
};
}  // namespace

static HAL_RuntimeType runtimeType{HAL_Runtime_Simulation};
static wpi::spinlock gOnShutdownMutex;
static std::vector<std::pair<void*, void (*)(void*)>> gOnShutdown;
static SimPeriodicCallbackRegistry gSimPeriodicBefore;
static SimPeriodicCallbackRegistry gSimPeriodicAfter;

namespace hal {
void InitializeDriverStation();
}  // namespace hal

namespace hal::init {
void InitializeHAL() {
  InitializeAccelerometerData();
  InitializeAddressableLEDData();
  InitializeAnalogGyroData();
  InitializeAnalogInData();
  InitializeAnalogOutData();
  InitializeAnalogTriggerData();
  InitializeCanData();
  InitializeCANAPI();
  InitializeDigitalPWMData();
  InitializeDutyCycleData();
  InitializeDIOData();
  InitializeDriverStationData();
  InitializeEncoderData();
  InitializeI2CData();
  InitializeCTREPCMData();
  InitializeREVPHData();
  InitializePowerDistributionData();
  InitializePWMData();
  InitializeRelayData();
  InitializeRoboRioData();
  InitializeSimDeviceData();
  InitializeSPIAccelerometerData();
  InitializeSPIData();
  InitializeAccelerometer();
  InitializeAddressableLED();
  InitializeAnalogAccumulator();
  InitializeAnalogGyro();
  InitializeAnalogInput();
  InitializeAnalogInternal();
  InitializeAnalogOutput();
  InitializeAnalogTrigger();
  InitializeCAN();
  InitializeConstants();
  InitializeCounter();
  InitializeDigitalInternal();
  InitializeDIO();
  InitializeDutyCycle();
  InitializeDriverStation();
  InitializeEncoder();
  InitializeExtensions();
  InitializeI2C();
  InitializeInterrupts();
  InitializeMain();
  InitializeMockHooks();
  InitializeNotifier();
  InitializePowerDistribution();
  InitializePorts();
  InitializePower();
  InitializeCTREPCM();
  InitializeREVPH();
  InitializePWM();
  InitializeRelay();
  InitializeSerialPort();
  InitializeSimDevice();
  InitializeSPI();
  InitializeThreads();
}
}  // namespace hal::init

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
    case HAL_CAN_TIMEOUT:
      return HAL_CAN_TIMEOUT_MESSAGE;
    case HAL_SIM_NOT_SUPPORTED:
      return HAL_SIM_NOT_SUPPORTED_MESSAGE;
    case HAL_CAN_BUFFER_OVERRUN:
      return HAL_CAN_BUFFER_OVERRUN_MESSAGE;
    case HAL_LED_CHANNEL_ERROR:
      return HAL_LED_CHANNEL_ERROR_MESSAGE;
    case HAL_USE_LAST_ERROR:
      return HAL_USE_LAST_ERROR_MESSAGE;
    case HAL_CONSOLE_OUT_ENABLED_ERROR:
      return HAL_CONSOLE_OUT_ENABLED_ERROR_MESSAGE;
    default:
      return "Unknown error status";
  }
}

HAL_RuntimeType HAL_GetRuntimeType(void) {
  return runtimeType;
}

void HALSIM_SetRuntimeType(HAL_RuntimeType type) {
  runtimeType = type;
}

int32_t HAL_GetFPGAVersion(int32_t* status) {
  return 2018;  // Automatically script this at some point
}

int64_t HAL_GetFPGARevision(int32_t* status) {
  return 0;  // TODO: Find a better number to return;
}

void HAL_GetSerialNumber(struct WPI_String* serialNumber) {
  HALSIM_GetRoboRioSerialNumber(serialNumber);
}

void HAL_GetComments(struct WPI_String* comments) {
  HALSIM_GetRoboRioComments(comments);
}

int32_t HAL_GetTeamNumber(void) {
  return HALSIM_GetRoboRioTeamNumber();
}

uint64_t HAL_GetFPGATime(int32_t* status) {
  return hal::GetFPGATime();
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
  return SimRoboRioData[0].fpgaButton;
}

HAL_Bool HAL_GetSystemActive(int32_t* status) {
  return HALSIM_GetDriverStationEnabled();
}

HAL_Bool HAL_GetBrownedOut(int32_t* status) {
  return false;  // Figure out if we need to detect a brownout condition
}

int32_t HAL_GetCommsDisableCount(int32_t* status) {
  return 0;
}

HAL_Bool HAL_GetRSLState(int32_t* status) {
  return false;
}

HAL_Bool HAL_GetSystemTimeValid(int32_t* status) {
  return true;
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

  hal::RestartTiming();
  hal::InitializeDriverStation();

  initialized = true;

// Set Timer Precision to 0.5ms on Windows
#ifdef _WIN32
  // Use timeGetDevCaps as well to prevent Java from interfering
  TIMECAPS tc;
  if (timeGetDevCaps(&tc, sizeof(tc)) == TIMERR_NOERROR) {
    UINT target = (std::max)(static_cast<UINT>(1), tc.wPeriodMin);
    timeBeginPeriod(target);
    std::atexit([]() {
      TIMECAPS tc;
      if (timeGetDevCaps(&tc, sizeof(tc)) == TIMERR_NOERROR) {
        UINT target = (std::max)(static_cast<UINT>(1), tc.wPeriodMin);
        timeEndPeriod(target);
      }
    });
  }
  // https://stackoverflow.com/questions/3141556/how-to-setup-timer-resolution-to-0-5-ms
  ULONG min, max, current;
  if (NtQueryTimerResolution(&min, &max, &current) == 0) {
    ULONG currentRes;
    if (NtSetTimerResolution(max, TRUE, &currentRes) == 0) {
      std::atexit([]() {
        ULONG currentRes;
        NtSetTimerResolution(0, FALSE, &currentRes);
      });
    }
  }

  // https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setprocessinformation
  // Enable HighQoS to achieve maximum performance, and turn off power saving.

  // https://forums.oculusvr.com/t5/General/SteamVR-has-fixed-the-problems-with-Windows-11/td-p/956413
  // Always honor Timer Resolution Requests. This is to ensure that the timer
  // resolution set-up above sticks through transitions of the main window (eg:
  // minimization).
  // This setting was introduced in Windows 11 and the definition is not
  // available in older headers.
#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
  const auto PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION = 0x4U;
#endif

  // Try both at once, and if that doesn't succeed, only set HighQoS
  PROCESS_POWER_THROTTLING_STATE PowerThrottling{};
  PowerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
  PowerThrottling.ControlMask =
      PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION |
      PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
  PowerThrottling.StateMask = 0;

  auto status =
      SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling,
                            &PowerThrottling, sizeof(PowerThrottling));

  // setting both failed, fall back to HighQoS only
  if (status == 0) {
    PowerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    PowerThrottling.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
    PowerThrottling.StateMask = 0;

    SetProcessInformation(GetCurrentProcess(), ProcessPowerThrottling,
                          &PowerThrottling, sizeof(PowerThrottling));
  }
#endif  // _WIN32

#ifndef _WIN32
  setlinebuf(stdin);
  setlinebuf(stdout);
#endif

  if (HAL_LoadExtensions() < 0) {
    return false;
  }

  return true;  // Add initialization if we need to at a later point
}

void HAL_Shutdown(void) {
  std::vector<std::pair<void*, void (*)(void*)>> funcs;
  {
    std::scoped_lock lock(gOnShutdownMutex);
    funcs.swap(gOnShutdown);
  }
  for (auto&& func : funcs) {
    func.second(func.first);
  }
}

void HAL_OnShutdown(void* param, void (*func)(void*)) {
  std::scoped_lock lock(gOnShutdownMutex);
  gOnShutdown.emplace_back(param, func);
}

void HAL_SimPeriodicBefore(void) {
  gSimPeriodicBefore();
}

void HAL_SimPeriodicAfter(void) {
  gSimPeriodicAfter();
}

int32_t HALSIM_RegisterSimPeriodicBeforeCallback(
    HALSIM_SimPeriodicCallback callback, void* param) {
  return gSimPeriodicBefore.Register(callback, param);
}

void HALSIM_CancelSimPeriodicBeforeCallback(int32_t uid) {
  gSimPeriodicBefore.Cancel(uid);
}

int32_t HALSIM_RegisterSimPeriodicAfterCallback(
    HALSIM_SimPeriodicCallback callback, void* param) {
  return gSimPeriodicAfter.Register(callback, param);
}

void HALSIM_CancelSimPeriodicAfterCallback(int32_t uid) {
  gSimPeriodicAfter.Cancel(uid);
}

void HALSIM_CancelAllSimPeriodicCallbacks(void) {
  gSimPeriodicBefore.Reset();
  gSimPeriodicAfter.Reset();
}

int64_t HAL_Report(int32_t resource, int32_t instanceNumber, int32_t context,
                   const char* feature) {
  return 0;  // Do nothing for now
}

}  // extern "C"
