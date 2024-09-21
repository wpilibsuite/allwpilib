// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogTrigger.h"

#include <memory>
#include <utility>

#include <hal/AnalogTrigger.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/AnalogInput.h"
#include "frc/DutyCycle.h"
#include "frc/Errors.h"

using namespace frc;

AnalogTrigger::AnalogTrigger(int channel)
    : AnalogTrigger(std::make_shared<AnalogInput>(channel)) {
  m_ownsAnalog = true;
  wpi::SendableRegistry::AddChild(this, m_analogInput.get());
}

AnalogTrigger::AnalogTrigger(AnalogInput& input)
    : AnalogTrigger{{&input, wpi::NullDeleter<AnalogInput>{}}} {}

AnalogTrigger::AnalogTrigger(AnalogInput* input)
    : AnalogTrigger{{input, wpi::NullDeleter<AnalogInput>{}}} {}

AnalogTrigger::AnalogTrigger(std::shared_ptr<AnalogInput> input)
    : m_analogInput{std::move(input)} {
  int32_t status = 0;
  m_trigger = HAL_InitializeAnalogTrigger(m_analogInput->m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analogInput->GetChannel());
  int index = GetIndex();

  HAL_Report(HALUsageReporting::kResourceType_AnalogTrigger, index + 1);
  wpi::SendableRegistry::AddLW(this, "AnalogTrigger", index);
}

AnalogTrigger::AnalogTrigger(DutyCycle& input)
    : AnalogTrigger{{&input, wpi::NullDeleter<DutyCycle>{}}} {}

AnalogTrigger::AnalogTrigger(DutyCycle* input)
    : AnalogTrigger{{input, wpi::NullDeleter<DutyCycle>{}}} {}

AnalogTrigger::AnalogTrigger(std::shared_ptr<DutyCycle> input)
    : m_dutyCycle{input} {
  int32_t status = 0;
  m_trigger = HAL_InitializeAnalogTriggerDutyCycle(input->m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_dutyCycle->GetSourceChannel());
  int index = GetIndex();

  HAL_Report(HALUsageReporting::kResourceType_AnalogTrigger, index + 1);
  wpi::SendableRegistry::AddLW(this, "AnalogTrigger", index);
}

void AnalogTrigger::SetLimitsVoltage(double lower, double upper) {
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsVoltage(m_trigger, lower, upper, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
}

void AnalogTrigger::SetLimitsDutyCycle(double lower, double upper) {
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsDutyCycle(m_trigger, lower, upper, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
}

void AnalogTrigger::SetLimitsRaw(int lower, int upper) {
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsRaw(m_trigger, lower, upper, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
}

void AnalogTrigger::SetAveraged(bool useAveragedValue) {
  int32_t status = 0;
  HAL_SetAnalogTriggerAveraged(m_trigger, useAveragedValue, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
}

void AnalogTrigger::SetFiltered(bool useFilteredValue) {
  int32_t status = 0;
  HAL_SetAnalogTriggerFiltered(m_trigger, useFilteredValue, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
}

int AnalogTrigger::GetIndex() const {
  int32_t status = 0;
  auto ret = HAL_GetAnalogTriggerFPGAIndex(m_trigger, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return ret;
}

bool AnalogTrigger::GetInWindow() {
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerInWindow(m_trigger, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return result;
}

bool AnalogTrigger::GetTriggerState() {
  int32_t status = 0;
  bool result = HAL_GetAnalogTriggerTriggerState(m_trigger, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return result;
}

std::shared_ptr<AnalogTriggerOutput> AnalogTrigger::CreateOutput(
    AnalogTriggerType type) const {
  return std::shared_ptr<AnalogTriggerOutput>(
      new AnalogTriggerOutput(*this, type));
}

void AnalogTrigger::InitSendable(wpi::SendableBuilder& builder) {
  if (m_ownsAnalog) {
    m_analogInput->InitSendable(builder);
  }
}

int AnalogTrigger::GetSourceChannel() const {
  if (m_analogInput) {
    return m_analogInput->GetChannel();
  } else if (m_dutyCycle) {
    return m_dutyCycle->GetSourceChannel();
  } else {
    return -1;
  }
}
