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
#include "HAL/cpp/Resource.h"
#include "handles/HandlesInternal.h"

using namespace hal;

extern "C" {
struct trigger_t {
  tAnalogTrigger* trigger;
  AnalogPort* port;
  uint32_t index;
};
typedef struct trigger_t AnalogTrigger;

static hal::Resource* triggers = nullptr;

void* initializeAnalogTrigger(HalPortHandle port_handle, uint32_t* index,
                              int32_t* status) {
  hal::Resource::CreateResourceObject(&triggers, tAnalogTrigger::kNumSystems);

  AnalogTrigger* trigger = new AnalogTrigger();
  trigger->port = (AnalogPort*)initializeAnalogInputPort(port_handle, status);
  if (*status != 0) {
    return nullptr;
  }
  trigger->index = triggers->Allocate("Analog Trigger");
  *index = trigger->index;
  // TODO: if (index == ~0ul) { CloneError(triggers); return; }

  trigger->trigger = tAnalogTrigger::create(trigger->index, status);
  trigger->trigger->writeSourceSelect_Channel(trigger->port->pin, status);
  return trigger;
}

void cleanAnalogTrigger(void* analog_trigger_pointer, int32_t* status) {
  AnalogTrigger* trigger = (AnalogTrigger*)analog_trigger_pointer;
  if (!trigger) return;
  triggers->Free(trigger->index);
  delete trigger->trigger;
  freeAnalogInputPort(trigger->port);
  delete trigger;
}

void setAnalogTriggerLimitsRaw(void* analog_trigger_pointer, int32_t lower,
                               int32_t upper, int32_t* status) {
  AnalogTrigger* trigger = (AnalogTrigger*)analog_trigger_pointer;
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
void setAnalogTriggerLimitsVoltage(void* analog_trigger_pointer, double lower,
                                   double upper, int32_t* status) {
  AnalogTrigger* trigger = (AnalogTrigger*)analog_trigger_pointer;
  if (lower > upper) {
    *status = ANALOG_TRIGGER_LIMIT_ORDER_ERROR;
  }
  // TODO: This depends on the averaged setting.  Only raw values will work as
  // is.
  trigger->trigger->writeLowerLimit(
      getAnalogVoltsToValue(trigger->port, lower, status), status);
  trigger->trigger->writeUpperLimit(
      getAnalogVoltsToValue(trigger->port, upper, status), status);
}

/**
 * Configure the analog trigger to use the averaged vs. raw values.
 * If the value is true, then the averaged value is selected for the analog
 * trigger, otherwise the immediate value is used.
 */
void setAnalogTriggerAveraged(void* analog_trigger_pointer,
                              bool useAveragedValue, int32_t* status) {
  AnalogTrigger* trigger = (AnalogTrigger*)analog_trigger_pointer;
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
void setAnalogTriggerFiltered(void* analog_trigger_pointer,
                              bool useFilteredValue, int32_t* status) {
  AnalogTrigger* trigger = (AnalogTrigger*)analog_trigger_pointer;
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
bool getAnalogTriggerInWindow(void* analog_trigger_pointer, int32_t* status) {
  AnalogTrigger* trigger = (AnalogTrigger*)analog_trigger_pointer;
  return trigger->trigger->readOutput_InHysteresis(trigger->index, status) != 0;
}

/**
 * Return the TriggerState output of the analog trigger.
 * True if above upper limit.
 * False if below lower limit.
 * If in Hysteresis, maintain previous state.
 * @return The TriggerState output of the analog trigger.
 */
bool getAnalogTriggerTriggerState(void* analog_trigger_pointer,
                                  int32_t* status) {
  AnalogTrigger* trigger = (AnalogTrigger*)analog_trigger_pointer;
  return trigger->trigger->readOutput_OverLimit(trigger->index, status) != 0;
}

/**
 * Get the state of the analog trigger output.
 * @return The state of the analog trigger output.
 */
bool getAnalogTriggerOutput(void* analog_trigger_pointer,
                            AnalogTriggerType type, int32_t* status) {
  AnalogTrigger* trigger = (AnalogTrigger*)analog_trigger_pointer;
  bool result = false;
  switch (type) {
    case kInWindow:
      result =
          trigger->trigger->readOutput_InHysteresis(trigger->index, status);
      break;  // XXX: Backport
    case kState:
      result = trigger->trigger->readOutput_OverLimit(trigger->index, status);
      break;  // XXX: Backport
    case kRisingPulse:
    case kFallingPulse:
      *status = ANALOG_TRIGGER_PULSE_OUTPUT_ERROR;
      return false;
  }
  return result;
}
}
