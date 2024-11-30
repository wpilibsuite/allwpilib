// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/AnalogTrigger.h"

#include <memory>

#include "AnalogInternal.h"
#include "ConstantsInternal.h"
#include "DutyCycleInternal.h"
#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/AnalogInput.h"
#include "hal/DutyCycle.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace {

struct AnalogTrigger {
  std::unique_ptr<tAnalogTrigger> trigger;
  HAL_Handle handle;
  uint8_t index;
};

}  // namespace

static LimitedHandleResource<HAL_AnalogTriggerHandle, AnalogTrigger,
                             kNumAnalogTriggers, HAL_HandleEnum::AnalogTrigger>*
    analogTriggerHandles;

namespace hal::init {
void InitializeAnalogTrigger() {
  static LimitedHandleResource<HAL_AnalogTriggerHandle, AnalogTrigger,
                               kNumAnalogTriggers,
                               HAL_HandleEnum::AnalogTrigger>
      atH;
  analogTriggerHandles = &atH;
}
}  // namespace hal::init

extern "C" {

HAL_AnalogTriggerHandle HAL_InitializeAnalogTrigger(
    HAL_AnalogInputHandle portHandle, int32_t* status) {
  hal::init::CheckInit();
  // ensure we are given a valid and active AnalogInput handle
  auto analog_port = analogInputHandles->Get(portHandle);
  if (analog_port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  HAL_AnalogTriggerHandle handle = analogTriggerHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto trigger = analogTriggerHandles->Get(handle);
  if (trigger == nullptr) {  // would only occur on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  trigger->handle = portHandle;
  trigger->index = static_cast<uint8_t>(getHandleIndex(handle));

  trigger->trigger.reset(tAnalogTrigger::create(trigger->index, status));
  trigger->trigger->writeSourceSelect_Channel(analog_port->channel, status);
  return handle;
}

HAL_AnalogTriggerHandle HAL_InitializeAnalogTriggerDutyCycle(
    HAL_DutyCycleHandle dutyCycleHandle, int32_t* status) {
  hal::init::CheckInit();
  // ensure we are given a valid and active DutyCycle handle
  auto dutyCycle = dutyCycleHandles->Get(dutyCycleHandle);
  if (dutyCycle == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  HAL_AnalogTriggerHandle handle = analogTriggerHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto trigger = analogTriggerHandles->Get(handle);
  if (trigger == nullptr) {  // would only occur on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  trigger->handle = dutyCycleHandle;
  trigger->index = static_cast<uint8_t>(getHandleIndex(handle));

  trigger->trigger.reset(tAnalogTrigger::create(trigger->index, status));
  trigger->trigger->writeSourceSelect_Channel(dutyCycle->index, status);
  trigger->trigger->writeSourceSelect_DutyCycle(true, status);
  return handle;
}

void HAL_CleanAnalogTrigger(HAL_AnalogTriggerHandle analogTriggerHandle) {
  analogTriggerHandles->Free(analogTriggerHandle);
  // caller owns the input handle.
}

void HAL_SetAnalogTriggerLimitsRaw(HAL_AnalogTriggerHandle analogTriggerHandle,
                                   int32_t lower, int32_t upper,
                                   int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (lower > upper) {
    *status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
    return;
  }
  trigger->trigger->writeLowerLimit(lower, status);
  trigger->trigger->writeUpperLimit(upper, status);
}

void HAL_SetAnalogTriggerLimitsDutyCycle(
    HAL_AnalogTriggerHandle analogTriggerHandle, double lower, double upper,
    int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (getHandleType(trigger->handle) != HAL_HandleEnum::DutyCycle) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (lower > upper) {
    *status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
    return;
  }

  if (lower < 0.0 || upper > 1.0) {
    *status = PARAMETER_OUT_OF_RANGE;
    auto lowerStr = std::to_string(lower);
    auto upperStr = std::to_string(upper);
    hal::SetLastError(
        status, "Lower must be >= 0 and upper must be <=1. Requested lower " +
                    lowerStr + " Requested upper " + upperStr);
    return;
  }

  trigger->trigger->writeLowerLimit(
      static_cast<int32_t>(kDutyCycleScaleFactor * lower), status);
  trigger->trigger->writeUpperLimit(
      static_cast<int32_t>(kDutyCycleScaleFactor * upper), status);
}

void HAL_SetAnalogTriggerLimitsVoltage(
    HAL_AnalogTriggerHandle analogTriggerHandle, double lower, double upper,
    int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (getHandleType(trigger->handle) != HAL_HandleEnum::AnalogInput) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (lower > upper) {
    *status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
    return;
  }

  // TODO: This depends on the averaged setting.  Only raw values will work as
  // is.
  trigger->trigger->writeLowerLimit(
      HAL_GetAnalogVoltsToValue(trigger->handle, lower, status), status);
  trigger->trigger->writeUpperLimit(
      HAL_GetAnalogVoltsToValue(trigger->handle, upper, status), status);
}

void HAL_SetAnalogTriggerAveraged(HAL_AnalogTriggerHandle analogTriggerHandle,
                                  HAL_Bool useAveragedValue, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (trigger->trigger->readSourceSelect_Filter(status) != 0 ||
      trigger->trigger->readSourceSelect_DutyCycle(status) != 0) {
    *status = INCOMPATIBLE_STATE;
    // TODO: wpi_setWPIErrorWithContext(IncompatibleMode, "Hardware does not
    // support average and filtering at the same time.");
  }
  trigger->trigger->writeSourceSelect_Averaged(useAveragedValue, status);
}

void HAL_SetAnalogTriggerFiltered(HAL_AnalogTriggerHandle analogTriggerHandle,
                                  HAL_Bool useFilteredValue, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (trigger->trigger->readSourceSelect_Averaged(status) != 0 ||
      trigger->trigger->readSourceSelect_DutyCycle(status) != 0) {
    *status = INCOMPATIBLE_STATE;
    // TODO: wpi_setWPIErrorWithContext(IncompatibleMode, "Hardware does not "
    // "support average and filtering at the same time.");
  }
  trigger->trigger->writeSourceSelect_Filter(useFilteredValue, status);
}

HAL_Bool HAL_GetAnalogTriggerInWindow(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return trigger->trigger->readOutput_InHysteresis(trigger->index, status) != 0;
}

HAL_Bool HAL_GetAnalogTriggerTriggerState(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return trigger->trigger->readOutput_OverLimit(trigger->index, status) != 0;
}

HAL_Bool HAL_GetAnalogTriggerOutput(HAL_AnalogTriggerHandle analogTriggerHandle,
                                    HAL_AnalogTriggerType type,
                                    int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool result = false;
  switch (type) {
    case HAL_Trigger_kInWindow:
      result =
          trigger->trigger->readOutput_InHysteresis(trigger->index, status);
      break;
    case HAL_Trigger_kState:
      result = trigger->trigger->readOutput_OverLimit(trigger->index, status);
      break;
    case HAL_Trigger_kRisingPulse:
    case HAL_Trigger_kFallingPulse:
      *status = ANALOG_TRIGGER_PULSE_OUTPUT_ERROR;
      return false;
      break;
  }
  return result;
}

int32_t HAL_GetAnalogTriggerFPGAIndex(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }
  return trigger->index;
}

}  // extern "C"
