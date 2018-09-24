/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogTrigger.h"

#include <utility>

#include <hal/HAL.h>

#include "frc/AnalogInput.h"
#include "frc/WPIErrors.h"

using namespace frc;

AnalogTrigger::AnalogTrigger(int channel)
    : AnalogTrigger(new AnalogInput(channel)) {
  m_ownsAnalog = true;
  AddChild(m_analogInput);
}

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
  SetName("AnalogTrigger", input->GetChannel());
}

AnalogTrigger::~AnalogTrigger() {
  int32_t status = 0;
  HAL_CleanAnalogTrigger(m_trigger, &status);

  if (m_ownsAnalog) {
    delete m_analogInput;
  }
}

AnalogTrigger::AnalogTrigger(AnalogTrigger&& rhs)
    : ErrorBase(std::move(rhs)),
      SendableBase(std::move(rhs)),
      m_index(std::move(rhs.m_index)) {
  std::swap(m_trigger, rhs.m_trigger);
  std::swap(m_analogInput, rhs.m_analogInput);
  std::swap(m_ownsAnalog, rhs.m_ownsAnalog);
}

AnalogTrigger& AnalogTrigger::operator=(AnalogTrigger&& rhs) {
  ErrorBase::operator=(std::move(rhs));
  SendableBase::operator=(std::move(rhs));

  m_index = std::move(rhs.m_index);
  std::swap(m_trigger, rhs.m_trigger);
  std::swap(m_analogInput, rhs.m_analogInput);
  std::swap(m_ownsAnalog, rhs.m_ownsAnalog);

  return *this;
}

void AnalogTrigger::SetLimitsVoltage(double lower, double upper) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsVoltage(m_trigger, lower, upper, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void AnalogTrigger::SetLimitsRaw(int lower, int upper) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsRaw(m_trigger, lower, upper, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void AnalogTrigger::SetAveraged(bool useAveragedValue) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerAveraged(m_trigger, useAveragedValue, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void AnalogTrigger::SetFiltered(bool useFilteredValue) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerFiltered(m_trigger, useFilteredValue, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

int AnalogTrigger::GetIndex() const {
  if (StatusIsFatal()) return -1;
  return m_index;
}

bool AnalogTrigger::GetInWindow() {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerInWindow(m_trigger, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return result;
}

bool AnalogTrigger::GetTriggerState() {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerTriggerState(m_trigger, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return result;
}

std::shared_ptr<AnalogTriggerOutput> AnalogTrigger::CreateOutput(
    AnalogTriggerType type) const {
  if (StatusIsFatal()) return nullptr;
  return std::shared_ptr<AnalogTriggerOutput>(
      new AnalogTriggerOutput(*this, type), NullDeleter<AnalogTriggerOutput>());
}

void AnalogTrigger::InitSendable(SendableBuilder& builder) {
  if (m_ownsAnalog) m_analogInput->InitSendable(builder);
}
