/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/AnalogTrigger.h"

#include "AnalogInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/AnalogInput.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "mockdata/AnalogInDataInternal.h"
#include "mockdata/AnalogTriggerDataInternal.h"

namespace {
struct AnalogTrigger {
  HAL_AnalogInputHandle analogHandle;
  uint8_t index;
  HAL_Bool trigState;
};
}  // namespace

using namespace hal;

static LimitedHandleResource<HAL_AnalogTriggerHandle, AnalogTrigger,
                             kNumAnalogTriggers, HAL_HandleEnum::AnalogTrigger>*
    analogTriggerHandles;

namespace hal {
namespace init {
void InitializeAnalogTrigger() {
  static LimitedHandleResource<HAL_AnalogTriggerHandle, AnalogTrigger,
                               kNumAnalogTriggers,
                               HAL_HandleEnum::AnalogTrigger>
      atH;
  analogTriggerHandles = &atH;
}
}  // namespace init
}  // namespace hal

int32_t hal::GetAnalogTriggerInputIndex(HAL_AnalogTriggerHandle handle,
                                        int32_t* status) {
  auto trigger = analogTriggerHandles->Get(handle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  auto analog_port = analogInputHandles->Get(trigger->analogHandle);
  if (analog_port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return -1;
  }

  return analog_port->channel;
}

extern "C" {

HAL_AnalogTriggerHandle HAL_InitializeAnalogTrigger(
    HAL_AnalogInputHandle portHandle, int32_t* index, int32_t* status) {
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
  trigger->analogHandle = portHandle;
  trigger->index = static_cast<uint8_t>(getHandleIndex(handle));
  *index = trigger->index;

  SimAnalogTriggerData[trigger->index].initialized = true;

  trigger->trigState = false;

  return handle;
}

void HAL_CleanAnalogTrigger(HAL_AnalogTriggerHandle analogTriggerHandle,
                            int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  analogTriggerHandles->Free(analogTriggerHandle);
  if (trigger == nullptr) return;
  SimAnalogTriggerData[trigger->index].initialized = false;
  // caller owns the analog input handle.
}

static double GetAnalogValueToVoltage(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t value,
    int32_t* status) {
  int32_t LSBWeight = HAL_GetAnalogLSBWeight(analogTriggerHandle, status);
  int32_t offset = HAL_GetAnalogOffset(analogTriggerHandle, status);

  double voltage = LSBWeight * 1.0e-9 * value - offset * 1.0e-9;
  return voltage;
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
  }

  double trigLower =
      GetAnalogValueToVoltage(trigger->analogHandle, lower, status);
  if (status != 0) return;
  double trigUpper =
      GetAnalogValueToVoltage(trigger->analogHandle, upper, status);
  if (status != 0) return;

  SimAnalogTriggerData[trigger->index].triggerUpperBound = trigUpper;
  SimAnalogTriggerData[trigger->index].triggerLowerBound = trigLower;
}
void HAL_SetAnalogTriggerLimitsVoltage(
    HAL_AnalogTriggerHandle analogTriggerHandle, double lower, double upper,
    int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (lower > upper) {
    *status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
  }

  SimAnalogTriggerData[trigger->index].triggerUpperBound = upper;
  SimAnalogTriggerData[trigger->index].triggerLowerBound = lower;
}
void HAL_SetAnalogTriggerAveraged(HAL_AnalogTriggerHandle analogTriggerHandle,
                                  HAL_Bool useAveragedValue, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  AnalogTriggerData* triggerData = &SimAnalogTriggerData[trigger->index];

  if (triggerData->triggerMode.Get() == HALSIM_AnalogTriggerFiltered) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  auto setVal = useAveragedValue ? HALSIM_AnalogTriggerAveraged
                                 : HALSIM_AnalogTriggerUnassigned;
  triggerData->triggerMode = setVal;
}
void HAL_SetAnalogTriggerFiltered(HAL_AnalogTriggerHandle analogTriggerHandle,
                                  HAL_Bool useFilteredValue, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  AnalogTriggerData* triggerData = &SimAnalogTriggerData[trigger->index];

  if (triggerData->triggerMode.Get() == HALSIM_AnalogTriggerAveraged) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  auto setVal = useFilteredValue ? HALSIM_AnalogTriggerAveraged
                                 : HALSIM_AnalogTriggerUnassigned;
  triggerData->triggerMode = setVal;
}

static double GetTriggerValue(AnalogTrigger* trigger, int32_t* status) {
  auto analogIn = analogInputHandles->Get(trigger->analogHandle);
  if (analogIn == nullptr) {
    // Returning HAL Handle Error, but going to ignore lower down
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  return SimAnalogInData[analogIn->channel].voltage;
}

HAL_Bool HAL_GetAnalogTriggerInWindow(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  double voltage = GetTriggerValue(trigger.get(), status);
  if (*status == HAL_HANDLE_ERROR) {
    // Don't error if analog has been destroyed
    *status = 0;
    return false;
  }

  double trigUpper = SimAnalogTriggerData[trigger->index].triggerUpperBound;
  double trigLower = SimAnalogTriggerData[trigger->index].triggerLowerBound;

  return voltage >= trigLower && voltage <= trigUpper;
}
HAL_Bool HAL_GetAnalogTriggerTriggerState(
    HAL_AnalogTriggerHandle analogTriggerHandle, int32_t* status) {
  auto trigger = analogTriggerHandles->Get(analogTriggerHandle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  double voltage = GetTriggerValue(trigger.get(), status);
  if (*status == HAL_HANDLE_ERROR) {
    // Don't error if analog has been destroyed
    *status = 0;
    return false;
  }

  double trigUpper = SimAnalogTriggerData[trigger->index].triggerUpperBound;
  double trigLower = SimAnalogTriggerData[trigger->index].triggerLowerBound;

  if (voltage < trigLower) {
    trigger->trigState = false;
    return false;
  }
  if (voltage > trigUpper) {
    trigger->trigState = true;
    return true;
  }
  return trigger->trigState;
}
HAL_Bool HAL_GetAnalogTriggerOutput(HAL_AnalogTriggerHandle analogTriggerHandle,
                                    HAL_AnalogTriggerType type,
                                    int32_t* status) {
  if (type == HAL_Trigger_kInWindow) {
    return HAL_GetAnalogTriggerInWindow(analogTriggerHandle, status);
  } else if (type == HAL_Trigger_kState) {
    return HAL_GetAnalogTriggerTriggerState(analogTriggerHandle, status);
  } else {
    *status = ANALOG_TRIGGER_PULSE_OUTPUT_ERROR;
    return false;
  }
}
}  // extern "C"
