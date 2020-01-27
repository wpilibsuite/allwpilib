/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/DutyCycle.h"

#include <memory>

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

static constexpr int32_t kScaleFactor = 4e7 - 1;

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
  return HAL_GetDutyCycleOutputRaw(dutyCycleHandle, status) /
         static_cast<double>(kScaleFactor);
}

int32_t HAL_GetDutyCycleOutputRaw(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status) {
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (!dutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  // TODO Handle Overflow
  unsigned char overflow = 0;
  return dutyCycle->dutyCycle->readOutput(&overflow, status);
}

int32_t HAL_GetDutyCycleOutputScaleFactor(HAL_DutyCycleHandle dutyCycleHandle,
                                          int32_t* status) {
  return kScaleFactor;
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
