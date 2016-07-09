/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/AnalogTrigger.h"

#include "AnalogInternal.h"
#include "HAL/AnalogInput.h"
#include "HAL/Errors.h"
#include "PortsInternal.h"
#include "handles/HandlesInternal.h"
#include "handles/LimitedHandleResource.h"

using namespace hal;

namespace {
struct AnalogTrigger {
  tAnalogTrigger* trigger;
  HAL_AnalogInputHandle analogHandle;
  uint32_t index;
};
}

static LimitedHandleResource<HAL_AnalogTriggerHandle, AnalogTrigger,
                             kNumAnalogTriggers, HAL_HandleEnum::AnalogTrigger>
    analogTriggerHandles;

extern "C" {

HAL_AnalogTriggerHandle HAL_InitializeAnalogTrigger(
    HAL_AnalogInputHandle port_handle, uint32_t* index, int32_t* status) {
  if (port_handle == HAL_kInvalidHandle) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }
  HAL_AnalogInputHandle handle = analogTriggerHandles.Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto trigger = analogTriggerHandles.Get(handle);
  trigger->analogHandle = port_handle;

  auto analog_port = analogInputHandles.Get(trigger->analogHandle);
  if (analog_port == nullptr) {  // would only error on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  *index = static_cast<uint32_t>(getHandleIndex(handle));
  trigger->index = *index;
  // TODO: if (index == ~0ul) { CloneError(triggers); return; }

  trigger->trigger = tAnalogTrigger::create(trigger->index, status);
  trigger->trigger->writeSourceSelect_Channel(analog_port->pin, status);
  return handle;
}

void HAL_CleanAnalogTrigger(HAL_AnalogTriggerHandle analog_trigger_handle,
                            int32_t* status) {
  auto trigger = analogTriggerHandles.Get(analog_trigger_handle);
  if (trigger == nullptr) {  // ignore status error
    return;
  }
  analogTriggerHandles.Free(analog_trigger_handle);
  // caller owns the analog input handle.
  delete trigger->trigger;
}

void HAL_SetAnalogTriggerLimitsRaw(
    HAL_AnalogTriggerHandle analog_trigger_handle, int32_t lower, int32_t upper,
    int32_t* status) {
  auto trigger = analogTriggerHandles.Get(analog_trigger_handle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (lower > upper) {
    *status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
  }
  trigger->trigger->writeLowerLimit(lower, status);
  trigger->trigger->writeUpperLimit(upper, status);
}

/**
 * Set the upper and lower limits of the analog trigger.
 * The limits are given as floating point voltage values.
 */
void HAL_SetAnalogTriggerLimitsVoltage(
    HAL_AnalogTriggerHandle analog_trigger_handle, double lower, double upper,
    int32_t* status) {
  auto trigger = analogTriggerHandles.Get(analog_trigger_handle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (lower > upper) {
    *status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
  }

  // TODO: This depends on the averaged setting.  Only raw values will work as
  // is.
  trigger->trigger->writeLowerLimit(
      HAL_GetAnalogVoltsToValue(trigger->analogHandle, lower, status), status);
  trigger->trigger->writeUpperLimit(
      HAL_GetAnalogVoltsToValue(trigger->analogHandle, upper, status), status);
}

/**
 * Configure the analog trigger to use the averaged vs. raw values.
 * If the value is true, then the averaged value is selected for the analog
 * trigger, otherwise the immediate value is used.
 */
void HAL_SetAnalogTriggerAveraged(HAL_AnalogTriggerHandle analog_trigger_handle,
                                  bool useAveragedValue, int32_t* status) {
  auto trigger = analogTriggerHandles.Get(analog_trigger_handle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (trigger->trigger->readSourceSelect_Filter(status) != 0) {
    *status = INCOMPATIBLE_STATE;
    // TODO: wpi_setWPIErrorWithContext(IncompatibleMode, "Hardware does not
    // support average and filtering at the same time.");
  }
  trigger->trigger->writeSourceSelect_Averaged(useAveragedValue, status);
}

/**
 * Configure the analog trigger to use a filtered value.
 * The analog trigger will operate with a 3 point average rejection filter. This
 * is designed to help with 360 degree pot applications for the period where the
 * pot crosses through zero.
 */
void HAL_SetAnalogTriggerFiltered(HAL_AnalogTriggerHandle analog_trigger_handle,
                                  bool useFilteredValue, int32_t* status) {
  auto trigger = analogTriggerHandles.Get(analog_trigger_handle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (trigger->trigger->readSourceSelect_Averaged(status) != 0) {
    *status = INCOMPATIBLE_STATE;
    // TODO: wpi_setWPIErrorWithContext(IncompatibleMode, "Hardware does not "
    // "support average and filtering at the same time.");
  }
  trigger->trigger->writeSourceSelect_Filter(useFilteredValue, status);
}

/**
 * Return the InWindow output of the analog trigger.
 * True if the analog input is between the upper and lower limits.
 * @return The InWindow output of the analog trigger.
 */
bool HAL_GetAnalogTriggerInWindow(HAL_AnalogTriggerHandle analog_trigger_handle,
                                  int32_t* status) {
  auto trigger = analogTriggerHandles.Get(analog_trigger_handle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return trigger->trigger->readOutput_InHysteresis(trigger->index, status) != 0;
}

/**
 * Return the TriggerState output of the analog trigger.
 * True if above upper limit.
 * False if below lower limit.
 * If in Hysteresis, maintain previous state.
 * @return The TriggerState output of the analog trigger.
 */
bool HAL_GetAnalogTriggerTriggerState(
    HAL_AnalogTriggerHandle analog_trigger_handle, int32_t* status) {
  auto trigger = analogTriggerHandles.Get(analog_trigger_handle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return trigger->trigger->readOutput_OverLimit(trigger->index, status) != 0;
}

/**
 * Get the state of the analog trigger output.
 * @return The state of the analog trigger output.
 */
bool HAL_GetAnalogTriggerOutput(HAL_AnalogTriggerHandle analog_trigger_handle,
                                HAL_AnalogTriggerType type, int32_t* status) {
  auto trigger = analogTriggerHandles.Get(analog_trigger_handle);
  if (trigger == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool result = false;
  switch (type) {
    case HAL_Trigger_kInWindow:
      result =
          trigger->trigger->readOutput_InHysteresis(trigger->index, status);
      break;  // XXX: Backport
    case HAL_Trigger_kState:
      result = trigger->trigger->readOutput_OverLimit(trigger->index, status);
      break;  // XXX: Backport
    case HAL_Trigger_kRisingPulse:
    case HAL_Trigger_kFallingPulse:
      *status = ANALOG_TRIGGER_PULSE_OUTPUT_ERROR;
      return false;
  }
  return result;
}
}
