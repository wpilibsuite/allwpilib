// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DutyCycle.h"

#include <utility>

#include <hal/DutyCycle.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/DigitalSource.h"
#include "frc/Errors.h"

using namespace frc;

DutyCycle::DutyCycle(DigitalSource* source)
    : m_source{source, wpi::NullDeleter<DigitalSource>()} {
  if (!m_source) {
    throw FRC_MakeError(err::NullParameter, "source");
  }
  InitDutyCycle();
}

DutyCycle::DutyCycle(DigitalSource& source)
    : m_source{&source, wpi::NullDeleter<DigitalSource>()} {
  InitDutyCycle();
}

DutyCycle::DutyCycle(std::shared_ptr<DigitalSource> source)
    : m_source{std::move(source)} {
  if (!m_source) {
    throw FRC_MakeError(err::NullParameter, "source");
  }
  InitDutyCycle();
}

void DutyCycle::InitDutyCycle() {
  int32_t status = 0;
  m_handle =
      HAL_InitializeDutyCycle(m_source->GetPortHandleForRouting(),
                              static_cast<HAL_AnalogTriggerType>(
                                  m_source->GetAnalogTriggerTypeForRouting()),
                              &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  int index = GetFPGAIndex();
  HAL_Report(HALUsageReporting::kResourceType_DutyCycle, index + 1);
  wpi::SendableRegistry::AddLW(this, "Duty Cycle", index);
}

int DutyCycle::GetFPGAIndex() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFPGAIndex(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

int DutyCycle::GetFrequency() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

double DutyCycle::GetOutput() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutput(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

units::second_t DutyCycle::GetHighTime() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleHighTime(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return units::nanosecond_t{static_cast<double>(retVal)};
}

unsigned int DutyCycle::GetOutputScaleFactor() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutputScaleFactor(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

int DutyCycle::GetSourceChannel() const {
  return m_source->GetChannel();
}

void DutyCycle::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Duty Cycle");
  builder.AddDoubleProperty(
      "Frequency", [this] { return this->GetFrequency(); }, nullptr);
  builder.AddDoubleProperty(
      "Output", [this] { return this->GetOutput(); }, nullptr);
}
