/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/HAL.h"

#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

#include "ErrorsInternal.h"
#include "HALInitializer.h"
#include "MockHooksInternal.h"
#include "hal/DriverStation.h"
#include "hal/Errors.h"
#include "hal/Extensions.h"
#include "hal/handles/HandlesInternal.h"
#include "mockdata/RoboRioDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeHAL() {
  InitializeAccelerometerData();
  InitializeAnalogGyroData();
  InitializeAnalogInData();
  InitializeAnalogOutData();
  InitializeAnalogTriggerData();
  InitializeCanData();
  InitializeCANAPI();
  InitializeDigitalPWMData();
  InitializeDIOData();
  InitializeDriverStationData();
  InitializeEncoderData();
  InitializeI2CData();
  InitializePCMData();
  InitializePDPData();
  InitializePWMData();
  InitializeRelayData();
  InitializeRoboRioData();
  InitializeSPIAccelerometerData();
  InitializeSPIData();
  InitializeAccelerometer();
  InitializeAnalogAccumulator();
  InitializeAnalogGyro();
  InitializeAnalogInput();
  InitializeAnalogInternal();
  InitializeAnalogOutput();
  InitializeCAN();
  InitializeCompressor();
  InitializeConstants();
  InitializeCounter();
  InitializeDigitalInternal();
  InitializeDIO();
  InitializeDriverStation();
  InitializeEncoder();
  InitializeExtensions();
  InitializeI2C();
  InitializeInterrupts();
  InitializeMockHooks();
  InitializeNotifier();
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
    case HAL_CAN_TIMEOUT:
      return HAL_CAN_TIMEOUT_MESSAGE;
    default:
      return "Unknown error status";
  }
}

HAL_RuntimeType HAL_GetRuntimeType(void) { return HAL_Mock; }

int32_t HAL_GetFPGAVersion(int32_t* status) {
  return 2018;  // Automatically script this at some point
}

int64_t HAL_GetFPGARevision(int32_t* status) {
  return 0;  // TODO: Find a better number to return;
}

uint64_t HAL_GetFPGATime(int32_t* status) { return hal::GetFPGATime(); }

HAL_Bool HAL_GetFPGAButton(int32_t* status) {
  return SimRoboRioData[0].fpgaButton;
}

HAL_Bool HAL_GetSystemActive(int32_t* status) {
  return true;  // Figure out if we need to handle this
}

HAL_Bool HAL_GetBrownedOut(int32_t* status) {
  return false;  // Figure out if we need to detect a brownout condition
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

  wpi::outs().SetUnbuffered();
  if (HAL_LoadExtensions() < 0) return false;
  hal::RestartTiming();
  HAL_InitializeDriverStation();

  initialized = true;
  return true;  // Add initialization if we need to at a later point
}

int64_t HAL_Report(int32_t resource, int32_t instanceNumber, int32_t context,
                   const char* feature) {
  return 0;  // Do nothing for now
}

}  // extern "C"
