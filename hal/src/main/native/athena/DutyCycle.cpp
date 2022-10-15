// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DutyCycle.h"

#include <memory>

#include "ConstantsInternal.h"
#include "DigitalInternal.h"
#include "DutyCycleInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/ChipObject.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace hal {
LimitedHandleResource<HAL_DutyCycleHandle, DutyCycle, kNumDutyCycles,
                      HAL_HandleEnum::DutyCycle>* dutyCycleHandles;
namespace init {
void InitializeDutyCycle() {
  static LimitedHandleResource<HAL_DutyCycleHandle, DutyCycle, kNumDutyCycles,
                               HAL_HandleEnum::DutyCycle>
      dcH;
  dutyCycleHandles = &dcH;
}
}  // namespace init
}  // namespace hal

extern "C" {
HAL_DutyCycleHandle HAL_InitializeDutyCycle(HAL_Handle digitalSourceHandle,
                                            HAL_AnalogTriggerType triggerType,
                                            int32_t* status) {
  hal::init::CheckInit();

  bool routingAnalogTrigger = false;
  uint8_t routingChannel = 0;
  uint8_t routingModule = 0;
  bool success =
      remapDigitalSource(digitalSourceHandle, triggerType, routingChannel,
                         routingModule, routingAnalogTrigger);

  if (!success) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  HAL_DutyCycleHandle handle = dutyCycleHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }

  auto dutyCycle = dutyCycleHandles->Get(handle);
  uint32_t index = static_cast<uint32_t>(getHandleIndex(handle));
  dutyCycle->dutyCycle.reset(tDutyCycle::create(index, status));

  dutyCycle->dutyCycle->writeSource_AnalogTrigger(routingAnalogTrigger, status);
  dutyCycle->dutyCycle->writeSource_Channel(routingChannel, status);
  dutyCycle->dutyCycle->writeSource_Module(routingModule, status);
  dutyCycle->index = index;

  return handle;
}
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle) {
  // Just free it, the unique ptr will take care of everything else
  dutyCycleHandles->Free(dutyCycleHandle);
}

void HAL_SetDutyCycleSimDevice(HAL_EncoderHandle handle,
                               HAL_SimDeviceHandle device) {}

int32_t HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (!dutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  // TODO Handle Overflow
  unsigned char overflow = 0;
  return dutyCycle->dutyCycle->readFrequency(&overflow, status);
}

double HAL_GetDutyCycleOutput(HAL_DutyCycleHandle dutyCycleHandle,
                              int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (!dutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  // TODO Handle Overflow
  unsigned char overflow = 0;
  uint32_t output = dutyCycle->dutyCycle->readOutput(&overflow, status);
  return output / static_cast<double>(kDutyCycleScaleFactor);
}

int32_t HAL_GetDutyCycleHighTime(HAL_DutyCycleHandle dutyCycleHandle,
                                 int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (!dutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  // TODO Handle Overflow
  unsigned char overflow = 0;
  uint32_t freq0 = dutyCycle->dutyCycle->readFrequency(&overflow, status);
  uint32_t output = dutyCycle->dutyCycle->readOutput(&overflow, status);
  uint32_t freq1 = dutyCycle->dutyCycle->readFrequency(&overflow, status);
  if (*status != 0) {
    return 0;
  }
  if (freq0 != freq1) {
    // Frequency rolled over. Reread output
    output = dutyCycle->dutyCycle->readOutput(&overflow, status);
    if (*status != 0) {
      return 0;
    }
  }
  if (freq1 == 0) {
    return 0;
  }
  // Output will be at max 4e7, so x25 will still fit in a 32 bit signed int.
  return (output / freq1) * 25;
}

int32_t HAL_GetDutyCycleOutputScaleFactor(HAL_DutyCycleHandle dutyCycleHandle,
                                          int32_t* status) {
  return kDutyCycleScaleFactor;
}

int32_t HAL_GetDutyCycleFPGAIndex(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (!dutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }
  return dutyCycle->index;
}
}  // extern "C"
