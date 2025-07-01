// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/AnalogPotentiometer.hpp"

#include <memory>
#include <utility>

#include "wpi/system/RobotController.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/NullDeleter.hpp"

using namespace wpi;

AnalogPotentiometer::AnalogPotentiometer(int channel, double fullRange,
                                         double offset)
    : AnalogPotentiometer(std::make_shared<AnalogInput>(channel), fullRange,
                          offset) {}

AnalogPotentiometer::AnalogPotentiometer(AnalogInput* input, double fullRange,
                                         double offset)
    : AnalogPotentiometer(std::shared_ptr<AnalogInput>(
                              input, wpi::util::NullDeleter<AnalogInput>()),
                          fullRange, offset) {}

AnalogPotentiometer::AnalogPotentiometer(std::shared_ptr<AnalogInput> input,
                                         double fullRange, double offset)
    : m_analog_input(std::move(input)),
      m_fullRange(fullRange),
      m_offset(offset) {}

double AnalogPotentiometer::Get() const {
  return (m_analog_input->GetVoltage() / RobotController::GetVoltage3V3()) *
             m_fullRange +
         m_offset;
}

void AnalogPotentiometer::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Value", Get());
}

std::string_view AnalogPotentiometer::GetTelemetryType() const {
  return "Analog Input";
}
