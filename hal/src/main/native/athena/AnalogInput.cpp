/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/AnalogInput.h"

#include <FRC_NetworkCommunication/AICalibration.h>
#include <wpi/mutex.h>

#include "AnalogInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/AnalogAccumulator.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {
namespace init {
void InitializeAnalogInput() {}
}  // namespace init
}  // namespace hal

using namespace hal;

extern "C" {

HAL_AnalogInputHandle HAL_InitializeAnalogInputPort(HAL_PortHandle portHandle,
                                                    int32_t* status) {
  hal::init::CheckInit();
  initializeAnalog(status);

  if (*status != 0) return HAL_kInvalidHandle;

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  HAL_AnalogInputHandle handle = analogInputHandles->Allocate(channel, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  // Initialize port structure
  auto analog_port = analogInputHandles->Get(handle);
  if (analog_port == nullptr) {  // would only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  analog_port->channel = static_cast<uint8_t>(channel);
  if (HAL_IsAccumulatorChannel(handle, status)) {
    analog_port->accumulator.reset(tAccumulator::create(channel, status));
  } else {
    analog_port->accumulator = nullptr;
  }

  // Set default configuration
  analogInputSystem->writeScanList(channel, channel, status);
  HAL_SetAnalogAverageBits(handle, kDefaultAverageBits, status);
  HAL_SetAnalogOversampleBits(handle, kDefaultOversampleBits, status);
  return handle;
}

void HAL_FreeAnalogInputPort(HAL_AnalogInputHandle analogPortHandle) {
  // no status, so no need to check for a proper free.
  analogInputHandles->Free(analogPortHandle);
}

HAL_Bool HAL_CheckAnalogModule(int32_t module) { return module == 1; }

HAL_Bool HAL_CheckAnalogInputChannel(int32_t channel) {
  return channel < kNumAnalogInputs && channel >= 0;
}

void HAL_SetAnalogSampleRate(double samplesPerSecond, int32_t* status) {
  // TODO: This will change when variable size scan lists are implemented.
  // TODO: Need double comparison with epsilon.
  // wpi_assert(!sampleRateSet || GetSampleRate() == samplesPerSecond);
  initializeAnalog(status);
  if (*status != 0) return;
  setAnalogSampleRate(samplesPerSecond, status);
}

double HAL_GetAnalogSampleRate(int32_t* status) {
  initializeAnalog(status);
  if (*status != 0) return 0;
  uint32_t ticksPerConversion = analogInputSystem->readLoopTiming(status);
  uint32_t ticksPerSample =
      ticksPerConversion * getAnalogNumActiveChannels(status);
  return static_cast<double>(kTimebase) / static_cast<double>(ticksPerSample);
}

void HAL_SetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                              int32_t bits, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  analogInputSystem->writeAverageBits(port->channel, static_cast<uint8_t>(bits),
                                      status);
}

int32_t HAL_GetAnalogAverageBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return kDefaultAverageBits;
  }
  uint8_t result = analogInputSystem->readAverageBits(port->channel, status);
  return result;
}

void HAL_SetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                 int32_t bits, int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  analogInputSystem->writeOversampleBits(port->channel,
                                         static_cast<uint8_t>(bits), status);
}

int32_t HAL_GetAnalogOversampleBits(HAL_AnalogInputHandle analogPortHandle,
                                    int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return kDefaultOversampleBits;
  }
  uint8_t result = analogInputSystem->readOversampleBits(port->channel, status);
  return result;
}

int32_t HAL_GetAnalogValue(HAL_AnalogInputHandle analogPortHandle,
                           int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  tAI::tReadSelect readSelect;
  readSelect.Channel = port->channel;
  readSelect.Averaged = false;

  std::lock_guard<wpi::mutex> lock(analogRegisterWindowMutex);
  analogInputSystem->writeReadSelect(readSelect, status);
  analogInputSystem->strobeLatchOutput(status);
  return static_cast<int16_t>(analogInputSystem->readOutput(status));
}

int32_t HAL_GetAnalogAverageValue(HAL_AnalogInputHandle analogPortHandle,
                                  int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  tAI::tReadSelect readSelect;
  readSelect.Channel = port->channel;
  readSelect.Averaged = true;

  std::lock_guard<wpi::mutex> lock(analogRegisterWindowMutex);
  analogInputSystem->writeReadSelect(readSelect, status);
  analogInputSystem->strobeLatchOutput(status);
  return static_cast<int32_t>(analogInputSystem->readOutput(status));
}

int32_t HAL_GetAnalogVoltsToValue(HAL_AnalogInputHandle analogPortHandle,
                                  double voltage, int32_t* status) {
  if (voltage > 5.0) {
    voltage = 5.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  if (voltage < 0.0) {
    voltage = 0.0;
    *status = VOLTAGE_OUT_OF_RANGE;
  }
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogPortHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogPortHandle, status);
  int32_t value =
      static_cast<int32_t>((voltage + offset * 1.0e-9) / (LSBWeight * 1.0e-9));
  return value;
}

double HAL_GetAnalogVoltage(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  int32_t value = HAL_GetAnalogValue(analogPortHandle, status);
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogPortHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogPortHandle, status);
  double voltage = LSBWeight * 1.0e-9 * value - offset * 1.0e-9;
  return voltage;
}

double HAL_GetAnalogAverageVoltage(HAL_AnalogInputHandle analogPortHandle,
                                   int32_t* status) {
  int32_t value = HAL_GetAnalogAverageValue(analogPortHandle, status);
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogPortHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogPortHandle, status);
  int32_t oversampleBits =
      HAL_GetAnalogOversampleBits(analogPortHandle, status);
  double voltage =
      LSBWeight * 1.0e-9 * value / static_cast<double>(1 << oversampleBits) -
      offset * 1.0e-9;
  return voltage;
}

int32_t HAL_GetAnalogLSBWeight(HAL_AnalogInputHandle analogPortHandle,
                               int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int32_t lsbWeight = FRC_NetworkCommunication_nAICalibration_getLSBWeight(
      0, port->channel, status);  // XXX: aiSystemIndex == 0?
  return lsbWeight;
}

int32_t HAL_GetAnalogOffset(HAL_AnalogInputHandle analogPortHandle,
                            int32_t* status) {
  auto port = analogInputHandles->Get(analogPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  int32_t offset = FRC_NetworkCommunication_nAICalibration_getOffset(
      0, port->channel, status);  // XXX: aiSystemIndex == 0?
  return offset;
}

}  // extern "C"
