// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SharpIR.h"

#include <algorithm>

#include <hal/UsageReporting.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/AnalogInput.h"

using namespace frc;

SharpIR SharpIR::GP2Y0A02YK0F(int channel) {
  return SharpIR(channel, 62.28, -1.092, 20_cm, 150_cm);
}

SharpIR SharpIR::GP2Y0A21YK0F(int channel) {
  return SharpIR(channel, 26.449, -1.226, 10_cm, 80_cm);
}

SharpIR SharpIR::GP2Y0A41SK0F(int channel) {
  return SharpIR(channel, 12.354, -1.07, 4_cm, 30_cm);
}

SharpIR SharpIR::GP2Y0A51SK0F(int channel) {
  return SharpIR(channel, 5.2819, -1.161, 2_cm, 15_cm);
}

SharpIR::SharpIR(int channel, double a, double b, double minCM, double maxCM)
    : m_sensor(channel), m_A(a), m_B(b), m_minCM(minCM), m_maxCM(maxCM) {
  HAL_ReportUsage("IO", channel, "SharpIR");
  wpi::SendableRegistry::Add(this, "SharpIR", channel);

  m_simDevice = hal::SimDevice("SharpIR", m_sensor.GetChannel());
  if (m_simDevice) {
    m_simRange = m_simDevice.CreateDouble("Range (m)", false, 0.0);
    m_sensor.SetSimDevice(m_simDevice);
  }
}

int SharpIR::GetChannel() const {
  return m_sensor.GetChannel();
}

units::meter_t SharpIR::GetRange() const {
  if (m_simRange) {
    return std::clamp(units::meter_t{m_simRange.Get()}, m_min, m_max);
  } else {
    // Don't allow zero/negative values
    auto v = std::max(m_sensor.GetVoltage(), 0.00001);

    return std::clamp(units::meter_t{m_A * std::pow(v, m_B) * 1e-2}, m_min,
                      m_max);
  }
}

void SharpIR::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Ultrasonic");
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetRange().value(); }, nullptr);
}
