// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/accelerometer/AnalogAccelerometer.hpp"

#include "wpi/hal/UsageReporting.h"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/NullDeleter.hpp"

using namespace wpi;

AnalogAccelerometer::AnalogAccelerometer(int channel)
    : AnalogAccelerometer(std::make_shared<AnalogInput>(channel)) {}

AnalogAccelerometer::AnalogAccelerometer(AnalogInput* channel)
    : m_analogInput(channel, wpi::util::NullDeleter<AnalogInput>()) {
  if (!channel) {
    throw WPILIB_MakeError(err::NullParameter, "channel");
  }
  InitAccelerometer();
}

AnalogAccelerometer::AnalogAccelerometer(std::shared_ptr<AnalogInput> channel)
    : m_analogInput(channel) {
  if (!channel) {
    throw WPILIB_MakeError(err::NullParameter, "channel");
  }
  InitAccelerometer();
}

double AnalogAccelerometer::GetAcceleration() const {
  return (m_analogInput->GetVoltage() - m_zeroGVoltage) / m_voltsPerG;
}

void AnalogAccelerometer::SetSensitivity(double sensitivity) {
  m_voltsPerG = sensitivity;
}

void AnalogAccelerometer::SetZero(double zero) {
  m_zeroGVoltage = zero;
}

void AnalogAccelerometer::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Value", GetAcceleration());
}

std::string_view AnalogAccelerometer::GetTelemetryType() const {
  return "Accelerometer";
}

void AnalogAccelerometer::InitAccelerometer() {
  HAL_ReportUsage("IO", m_analogInput->GetChannel(), "Accelerometer");
}
