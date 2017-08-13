/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogTrigger.h"

#include <memory>

#include "AnalogInput.h"
#include "HAL/HAL.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Constructor for an analog trigger given a channel number.
 *
 * @param channel The channel number on the roboRIO to represent. 0-3 are
 *                on-board 4-7 are on the MXP port.
 */
AnalogTrigger::AnalogTrigger(int channel)
    : AnalogTrigger(new AnalogInput(channel)) {
  m_ownsAnalog = true;
}

/**
 * Construct an analog trigger given an analog input.
 *
 * This should be used in the case of sharing an analog channel between the
 * trigger and an analog input object.
 *
 * @param channel The pointer to the existing AnalogInput object
 */
AnalogTrigger::AnalogTrigger(AnalogInput* input) {
  m_analogInput = input;
  int32_t status = 0;
  int index = 0;
  m_trigger = HAL_InitializeAnalogTrigger(input->m_port, &index, &status);
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    m_index = std::numeric_limits<int>::max();
    m_trigger = HAL_kInvalidHandle;
    return;
  }
  m_index = index;

  HAL_Report(HALUsageReporting::kResourceType_AnalogTrigger, input->m_channel);
}

AnalogTrigger::~AnalogTrigger() {
  int32_t status = 0;
  HAL_CleanAnalogTrigger(m_trigger, &status);

  if (m_ownsAnalog && m_analogInput != nullptr) {
    delete m_analogInput;
  }
}

/**
 * Set the upper and lower limits of the analog trigger.
 *
 * The limits are given in ADC codes.  If oversampling is used, the units must
 * be scaled appropriately.
 *
 * @param lower The lower limit of the trigger in ADC codes (12-bit values).
 * @param upper The upper limit of the trigger in ADC codes (12-bit values).
 */
void AnalogTrigger::SetLimitsRaw(int lower, int upper) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsRaw(m_trigger, lower, upper, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Set the upper and lower limits of the analog trigger.
 *
 * The limits are given as floating point voltage values.
 *
 * @param lower The lower limit of the trigger in Volts.
 * @param upper The upper limit of the trigger in Volts.
 */
void AnalogTrigger::SetLimitsVoltage(double lower, double upper) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsVoltage(m_trigger, lower, upper, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Configure the analog trigger to use the averaged vs. raw values.
 *
 * If the value is true, then the averaged value is selected for the analog
 * trigger, otherwise the immediate value is used.
 *
 * @param useAveragedValue If true, use the Averaged value, otherwise use the
 *                         instantaneous reading
 */
void AnalogTrigger::SetAveraged(bool useAveragedValue) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerAveraged(m_trigger, useAveragedValue, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Configure the analog trigger to use a filtered value.
 *
 * The analog trigger will operate with a 3 point average rejection filter. This
 * is designed to help with 360 degree pot applications for the period where
 * the pot crosses through zero.
 *
 * @param useFilteredValue If true, use the 3 point rejection filter, otherwise
 *                         use the unfiltered value
 */
void AnalogTrigger::SetFiltered(bool useFilteredValue) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerFiltered(m_trigger, useFilteredValue, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

/**
 * Return the index of the analog trigger.
 *
 * This is the FPGA index of this analog trigger instance.
 *
 * @return The index of the analog trigger.
 */
int AnalogTrigger::GetIndex() const {
  if (StatusIsFatal()) return -1;
  return m_index;
}

/**
 * Return the InWindow output of the analog trigger.
 *
 * True if the analog input is between the upper and lower limits.
 *
 * @return True if the analog input is between the upper and lower limits.
 */
bool AnalogTrigger::GetInWindow() {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerInWindow(m_trigger, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return result;
}

/**
 * Return the TriggerState output of the analog trigger.
 *
 * True if above upper limit.
 * False if below lower limit.
 * If in Hysteresis, maintain previous state.
 *
 * @return True if above upper limit. False if below lower limit. If in
 *         Hysteresis, maintain previous state.
 */
bool AnalogTrigger::GetTriggerState() {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerTriggerState(m_trigger, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return result;
}

/**
 * Creates an AnalogTriggerOutput object.
 *
 * Gets an output object that can be used for routing.
 * Caller is responsible for deleting the AnalogTriggerOutput object.
 *
 * @param type An enum of the type of output object to create.
 * @return A pointer to a new AnalogTriggerOutput object.
 */
std::shared_ptr<AnalogTriggerOutput> AnalogTrigger::CreateOutput(
    AnalogTriggerType type) const {
  if (StatusIsFatal()) return nullptr;
  return std::shared_ptr<AnalogTriggerOutput>(
      new AnalogTriggerOutput(*this, type), NullDeleter<AnalogTriggerOutput>());
}
