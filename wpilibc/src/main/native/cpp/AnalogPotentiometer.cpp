// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogPotentiometer.h"

#include <memory>
#include <utility>

#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/RobotController.h"

using namespace frc;

AnalogPotentiometer::AnalogPotentiometer(int channel, double fullRange,
                                         double offset)
    : AnalogPotentiometer(std::make_shared<AnalogInput>(channel), fullRange,
                          offset) {
  wpi::SendableRegistry::AddChild(this, m_analog_input.get());
}

AnalogPotentiometer::AnalogPotentiometer(AnalogInput* input, double fullRange,
                                         double offset)
    : AnalogPotentiometer(
          std::shared_ptr<AnalogInput>(input, wpi::NullDeleter<AnalogInput>()),
          fullRange, offset) {}

AnalogPotentiometer::AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                                         double fullRange, double offset)
    : m_analog_input(std::move(input)),
      m_fullRange(fullRange),
      m_offset(offset) {
  wpi::SendableRegistry::AddLW(this, "AnalogPotentiometer",
                               m_analog_input->GetChannel());
}

double AnalogPotentiometer::Get() const {
  return (m_analog_input->GetAverageVoltage() /
          RobotController::GetVoltage5V()) *
             m_fullRange +
         m_offset;
}

void AnalogPotentiometer::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Input");
  builder.AddDoubleProperty("Value", [=, this] { return Get(); }, nullptr);
}
