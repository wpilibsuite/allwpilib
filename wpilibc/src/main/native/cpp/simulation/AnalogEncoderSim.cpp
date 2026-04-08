// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AnalogEncoderSim.hpp"

#include "wpi/hardware/rotation/AnalogEncoder.hpp"
#include "wpi/simulation/SimDeviceSim.hpp"

using namespace wpi::sim;

AnalogEncoderSim::AnalogEncoderSim(const wpi::AnalogEncoder& encoder) {
  wpi::sim::SimDeviceSim deviceSim{"AnalogEncoder", encoder.GetChannel()};
  m_positionSim = deviceSim.GetDouble("Position");
}

void AnalogEncoderSim::Set(double value) {
  m_positionSim.Set(value);
}

double AnalogEncoderSim::Get() {
  return m_positionSim.Get();
}
