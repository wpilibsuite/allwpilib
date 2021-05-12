// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogPotentiometer.h"

#include <utility>

#include "frc/Base.h"
#include "frc/RobotController.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

AnalogPotentiometer::AnalogPotentiometer(int channel, double fullRange,
                                         double offset)
    : AnalogPotentiometer(std::make_shared<AnalogInput>(channel), fullRange,
                          offset) {
  SendableRegistry::AddChild(this, m_analog_input.get());
}

AnalogPotentiometer::AnalogPotentiometer(AnalogInput* input, double fullRange,
                                         double offset)
    : AnalogPotentiometer(
          std::shared_ptr<AnalogInput>(input, NullDeleter<AnalogInput>()),
          fullRange, offset) {}

AnalogPotentiometer::AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                                         double fullRange, double offset)
    : m_analog_input(std::move(input)),
      m_fullRange(fullRange),
      m_offset(offset) {
  SendableRegistry::AddLW(this, "AnalogPotentiometer",
                          m_analog_input->GetChannel());
}

double AnalogPotentiometer::Get() const {
  return (m_analog_input->GetAverageVoltage() /
          RobotController::GetVoltage5V()) *
             m_fullRange +
         m_offset;
}

void AnalogPotentiometer::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Input");
  builder.AddDoubleProperty(
      "Value", [=]() { return Get(); }, nullptr);
}
