// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/accelerometer/AnalogAccelerometer.hpp"

#include "wpi/hal/UsageReporting.h"
#include "wpi/util/NullDeleter.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

#include "wpi/system/Errors.hpp"

using namespace frc;

AnalogAccelerometer::AnalogAccelerometer(int channel)
    : AnalogAccelerometer(std::make_shared<AnalogInput>(channel)) {
  wpi::SendableRegistry::AddChild(this, m_analogInput.get());
}

AnalogAccelerometer::AnalogAccelerometer(AnalogInput* channel)
    : m_analogInput(channel, wpi::NullDeleter<AnalogInput>()) {
  if (!channel) {
    throw FRC_MakeError(err::NullParameter, "channel");
  }
  InitAccelerometer();
}

AnalogAccelerometer::AnalogAccelerometer(std::shared_ptr<AnalogInput> channel)
    : m_analogInput(channel) {
  if (!channel) {
    throw FRC_MakeError(err::NullParameter, "channel");
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

void AnalogAccelerometer::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Accelerometer");
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetAcceleration(); }, nullptr);
}

void AnalogAccelerometer::InitAccelerometer() {
  HAL_ReportUsage("IO", m_analogInput->GetChannel(), "Accelerometer");

  wpi::SendableRegistry::Add(this, "Accelerometer",
                             m_analogInput->GetChannel());
}
