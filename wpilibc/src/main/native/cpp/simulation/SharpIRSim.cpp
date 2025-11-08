// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/SharpIRSim.hpp"

#include "wpi/hal/SimDevice.h"
#include "wpi/simulation/SimDeviceSim.hpp"
#include "wpi/units/length.hpp"

using namespace frc;

SharpIRSim::SharpIRSim(const SharpIR& sharpIR)
    : SharpIRSim(sharpIR.GetChannel()) {}

SharpIRSim::SharpIRSim(int channel) {
  frc::sim::SimDeviceSim deviceSim{"SharpIR", channel};
  m_simRange = deviceSim.GetDouble("Range (m)");
}

void SharpIRSim::SetRange(units::meter_t range) {
  m_simRange.Set(range.value());
}
