// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DIO.h"

#include "DigitalInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "mockdata/DIODataInternal.h"
#include "mockdata/DigitalPWMDataInternal.h"

using namespace hal;

static LimitedHandleResource<HAL_DigitalPWMHandle, uint8_t,
                             kNumDigitalPWMOutputs, HAL_HandleEnum::DigitalPWM>*
    digitalPWMHandles;

namespace hal::init {
void InitializeDIO() {
  static LimitedHandleResource<HAL_DigitalPWMHandle, uint8_t,
                               kNumDigitalPWMOutputs,
                               HAL_HandleEnum::DigitalPWM>
      dpH;
  digitalPWMHandles = &dpH;
}
}  // namespace hal::init

extern "C" {

HAL_DigitalHandle HAL_InitializeDIOPort(HAL_PortHandle portHandle,
                                        HAL_Bool input,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DIO", 0,
                                     kNumDigitalChannels, channel);
    return HAL_kInvalidHandle;
  }

  HAL_DigitalHandle handle;

  auto port = digitalChannelHandles->Allocate(channel, HAL_HandleEnum::DIO,
                                              &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "PWM or DIO", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for DIO", 0,
                                       kNumDigitalChannels, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
  }

  port->channel = static_cast<uint8_t>(channel);

  SimDIOData[channel].initialized = true;
  SimDIOData[channel].isInput = input;
  SimDIOData[channel].simDevice = 0;
  SimDIOData[channel].value = true;
  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

HAL_Bool HAL_CheckDIOChannel(int32_t channel) {
  return channel < kNumDigitalChannels && channel >= 0;
}

void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  // no status, so no need to check for a proper free.
  digitalChannelHandles->Free(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    return;
  }
  SimDIOData[port->channel].initialized = false;
}

void HAL_SetDIOSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device) {
  auto port = digitalChannelHandles->Get(handle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    return;
  }
  SimDIOData[port->channel].simDevice = device;
}

HAL_DigitalPWMHandle HAL_AllocateDigitalPWM(int32_t* status) {
  auto handle = digitalPWMHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  auto id = digitalPWMHandles->Get(handle);
  if (id == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  *id = static_cast<uint8_t>(getHandleIndex(handle));

  SimDigitalPWMData[*id].initialized = true;

  return handle;
}

void HAL_FreeDigitalPWM(HAL_DigitalPWMHandle pwmGenerator) {
  auto port = digitalPWMHandles->Get(pwmGenerator);
  digitalPWMHandles->Free(pwmGenerator);
  if (port == nullptr) {
    return;
  }
  int32_t id = *port;
  SimDigitalPWMData[id].initialized = false;
}

void HAL_SetDigitalPWMRate(double rate, int32_t* status) {
  // Currently rounding in the log rate domain... heavy weight toward picking a
  // higher freq.
  // TODO: Round in the linear rate domain.
  // uint8_t pwmPeriodPower = static_cast<uint8_t>(
  //     std::log2(1.0 / (kExpectedLoopTiming * 0.25E-6 * rate)) + 0.5);
  // TODO(THAD) : Add a case to set this in the simulator
  // digitalSystem->writePWMPeriodPower(pwmPeriodPower, status);
}

void HAL_SetDigitalPWMDutyCycle(HAL_DigitalPWMHandle pwmGenerator,
                                double dutyCycle, int32_t* status) {
  auto port = digitalPWMHandles->Get(pwmGenerator);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  int32_t id = *port;
  if (dutyCycle > 1.0) {
    dutyCycle = 1.0;
  }
  if (dutyCycle < 0.0) {
    dutyCycle = 0.0;
  }
  SimDigitalPWMData[id].dutyCycle = dutyCycle;
}

void HAL_SetDigitalPWMPPS(HAL_DigitalPWMHandle pwmGenerator, double dutyCycle,
                          int32_t* status) {
  auto port = digitalPWMHandles->Get(pwmGenerator);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  int32_t id = *port;
  if (dutyCycle > 1.0) {
    dutyCycle = 1.0;
  }
  if (dutyCycle < 0.0) {
    dutyCycle = 0.0;
  }
  SimDigitalPWMData[id].dutyCycle = dutyCycle;
}

void HAL_SetDigitalPWMOutputChannel(HAL_DigitalPWMHandle pwmGenerator,
                                    int32_t channel, int32_t* status) {
  auto port = digitalPWMHandles->Get(pwmGenerator);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  int32_t id = *port;
  SimDigitalPWMData[id].pin = channel;
}

void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (value != 0 && value != 1) {
    if (value != 0) {
      value = 1;
    }
  }
  if (SimDIOData[port->channel].isInput) {
    *status = PARAMETER_OUT_OF_RANGE;
    hal::SetLastError(status, "Cannot set output of an input channel");
    return;
  }
  SimDIOData[port->channel].value = value;
}

void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimDIOData[port->channel].isInput = input;
}

HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  HAL_Bool value = SimDIOData[port->channel].value;
  if (value > 1) {
    value = 1;
  }
  if (value < 0) {
    value = 0;
  }
  return value;
}

HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  HAL_Bool value = SimDIOData[port->channel].isInput;
  if (value > 1) {
    value = 1;
  }
  if (value < 0) {
    value = 0;
  }
  return value;
}

void HAL_Pulse(HAL_DigitalHandle dioPortHandle, double pulseLengthSeconds,
               int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  // TODO (Thad) Add this
}

void HAL_PulseMultiple(uint32_t channelMask, double pulseLengthSeconds,
                       int32_t* status) {
  // TODO (Thad) Add this
}

HAL_Bool HAL_IsPulsing(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return false;
  // TODO (Thad) Add this
}

HAL_Bool HAL_IsAnyPulsing(int32_t* status) {
  return false;  // TODO(Thad) Figure this out
}

void HAL_SetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t filterIndex,
                         int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  // mimics athena HAL
  port->filterIndex = filterIndex % 4;
}

int32_t HAL_GetFilterSelect(HAL_DigitalHandle dioPortHandle, int32_t* status) {
  auto port = digitalChannelHandles->Get(dioPortHandle, HAL_HandleEnum::DIO);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }
  return port->filterIndex;
}

void HAL_SetFilterPeriod(int32_t filterIndex, int64_t value, int32_t* status) {
  // TODO(Thad) figure this out
}

int64_t HAL_GetFilterPeriod(int32_t filterIndex, int32_t* status) {
  return 0;  // TODO(Thad) figure this out
}
}  // extern "C"
