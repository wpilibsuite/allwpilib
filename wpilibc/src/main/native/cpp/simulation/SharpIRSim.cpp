// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/SharpIRSim.h"

#include <hal/SimDevice.h>
#include <units/length.h>

#include "frc/simulation/SimDeviceSim.h"

using namespace frc;

SharpIRSim::SharpIRSim(const SharpIR& sharpIR)
    : SharpIRSim(sharpIR.GetChannel()) {}

SharpIRSim::SharpIRSim(int channel) {
  frc::sim::SimDeviceSim deviceSim{"SharpIR", channel};
  m_simRange = deviceSim.GetDouble("Range (cm)");
}

void SharpIRSim::SetRange(units::centimeter_t rng) {
  m_simRange.Set(rng.value());
}
