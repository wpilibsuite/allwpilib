// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SharpIR.h"

#include <algorithm>

#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/AnalogInput.h"

using namespace frc;

SharpIR SharpIR::GP2Y0A02YK0F(int channel) {
  return SharpIR(channel, 62.28, -1.092, 20, 150.0);
}

SharpIR SharpIR::GP2Y0A21YK0F(int channel) {
  return SharpIR(channel, 26.449, -1.226, 10.0, 80.0);
}

SharpIR SharpIR::GP2Y0A41SK0F(int channel) {
  return SharpIR(channel, 12.354, -1.07, 4.0, 30.0);
}

SharpIR SharpIR::GP2Y0A51SK0F(int channel) {
  return SharpIR(channel, 5.2819, -1.161, 2.0, 15.0);
}

SharpIR::SharpIR(int channel, double a, double b, double minCM, double maxCM)
    : m_sensor(channel), m_A(a), m_B(b), m_minCM(minCM), m_maxCM(maxCM) {
  wpi::SendableRegistry::AddLW(this, "SharpIR", channel);

  m_simDevice = hal::SimDevice("SharpIR", m_sensor.GetChannel());
  if (m_simDevice) {
    m_simRange = m_simDevice.CreateDouble("Range (cm)", false, 0.0);
    m_sensor.SetSimDevice(m_simDevice);
  }
}

int SharpIR::GetChannel() const {
  return m_sensor.GetChannel();
}

units::centimeter_t SharpIR::GetRange() const {
  double distance;

  if (m_simRange) {
    distance = m_simRange.Get();
  } else {
    // Don't allow zero/negative values
    auto v = std::max(m_sensor.GetVoltage(), 0.00001);
    distance = m_A * std::pow(v, m_B);
  }

  // Always constrain output
  return units::centimeter_t{std::max(std::min(distance, m_maxCM), m_minCM)};
}

void SharpIR::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Ultrasonic");
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetRange().value(); }, nullptr);
}
