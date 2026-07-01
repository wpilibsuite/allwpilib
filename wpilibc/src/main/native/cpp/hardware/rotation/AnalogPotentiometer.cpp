// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/AnalogPotentiometer.hpp"

#include <memory>
#include <utility>

#include "wpi/system/RobotController.hpp"
#include "wpi/util/NullDeleter.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

AnalogPotentiometer::AnalogPotentiometer(int channel, double fullRange,
                                         double offset)
    : AnalogPotentiometer(std::make_shared<AnalogInput>(channel), fullRange,
                          offset) {
  wpi::util::SendableRegistry::AddChild(this, m_analog_input.get());
}

AnalogPotentiometer::AnalogPotentiometer(AnalogInput* input, double fullRange,
                                         double offset)
    : AnalogPotentiometer(std::shared_ptr<AnalogInput>(
                              input, wpi::util::NullDeleter<AnalogInput>()),
                          fullRange, offset) {}

AnalogPotentiometer::AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                                         double fullRange, double offset)
    : m_analog_input(std::move(input)),
      m_fullRange(fullRange),
      m_offset(offset) {
  wpi::util::SendableRegistry::Add(this, "AnalogPotentiometer",
                                   m_analog_input->GetChannel());
}

double AnalogPotentiometer::Get() const {
  return (m_analog_input->GetVoltage() / RobotController::GetVoltage3V3()) *
             m_fullRange +
         m_offset;
}

void AnalogPotentiometer::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Input");
  builder.AddDoubleProperty("Value", [=, this] { return Get(); }, nullptr);
}
