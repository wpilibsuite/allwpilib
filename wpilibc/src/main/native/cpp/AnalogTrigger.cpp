/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogTrigger.h"

#include <utility>

#include <hal/FRCUsageReporting.h>

#include "frc/AnalogInput.h"
#include "frc/Base.h"
#include "frc/DutyCycle.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

AnalogTrigger::AnalogTrigger(int channel)
    : AnalogTrigger(new AnalogInput(channel)) {
  m_ownsAnalog = true;
  SendableRegistry::GetInstance().AddChild(this, m_analogInput);
}

AnalogTrigger::AnalogTrigger(AnalogInput* input) {
  m_analogInput = input;
  int32_t status = 0;
  m_trigger = HAL_InitializeAnalogTrigger(input->m_port, &status);
  if (status != 0) {
    wpi_setHALError(status);
    m_trigger = HAL_kInvalidHandle;
    return;
  }
  int index = GetIndex();

  HAL_Report(HALUsageReporting::kResourceType_AnalogTrigger, index + 1);
  SendableRegistry::GetInstance().AddLW(this, "AnalogTrigger", index);
}

AnalogTrigger::AnalogTrigger(DutyCycle* input) {
  m_dutyCycle = input;
  int32_t status = 0;
  m_trigger = HAL_InitializeAnalogTriggerDutyCycle(input->m_handle, &status);
  if (status != 0) {
    wpi_setHALError(status);
    m_trigger = HAL_kInvalidHandle;
    return;
  }
  int index = GetIndex();

  HAL_Report(HALUsageReporting::kResourceType_AnalogTrigger, index + 1);
  SendableRegistry::GetInstance().AddLW(this, "AnalogTrigger", index);
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
      SendableHelper(std::move(rhs)),
      m_trigger(std::move(rhs.m_trigger)) {
  std::swap(m_analogInput, rhs.m_analogInput);
  std::swap(m_dutyCycle, rhs.m_dutyCycle);
  std::swap(m_ownsAnalog, rhs.m_ownsAnalog);
}

AnalogTrigger& AnalogTrigger::operator=(AnalogTrigger&& rhs) {
  ErrorBase::operator=(std::move(rhs));
  SendableHelper::operator=(std::move(rhs));

  m_trigger = std::move(rhs.m_trigger);
  std::swap(m_analogInput, rhs.m_analogInput);
  std::swap(m_dutyCycle, rhs.m_dutyCycle);
  std::swap(m_ownsAnalog, rhs.m_ownsAnalog);

  return *this;
}

void AnalogTrigger::SetLimitsVoltage(double lower, double upper) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsVoltage(m_trigger, lower, upper, &status);
  wpi_setHALError(status);
}

void AnalogTrigger::SetLimitsDutyCycle(double lower, double upper) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsDutyCycle(m_trigger, lower, upper, &status);
  wpi_setHALError(status);
}

void AnalogTrigger::SetLimitsRaw(int lower, int upper) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsRaw(m_trigger, lower, upper, &status);
  wpi_setHALError(status);
}

void AnalogTrigger::SetAveraged(bool useAveragedValue) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerAveraged(m_trigger, useAveragedValue, &status);
  wpi_setHALError(status);
}

void AnalogTrigger::SetFiltered(bool useFilteredValue) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogTriggerFiltered(m_trigger, useFilteredValue, &status);
  wpi_setHALError(status);
}

int AnalogTrigger::GetIndex() const {
  if (StatusIsFatal()) return -1;
  int32_t status = 0;
  auto ret = HAL_GetAnalogTriggerFPGAIndex(m_trigger, &status);
  wpi_setHALError(status);
  return ret;
}

bool AnalogTrigger::GetInWindow() {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerInWindow(m_trigger, &status);
  wpi_setHALError(status);
  return result;
}

bool AnalogTrigger::GetTriggerState() {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerTriggerState(m_trigger, &status);
  wpi_setHALError(status);
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
