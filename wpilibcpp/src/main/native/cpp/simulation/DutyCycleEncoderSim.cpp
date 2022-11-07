// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DutyCycleEncoderSim.h"

#include "frc/DutyCycleEncoder.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

DutyCycleEncoderSim::DutyCycleEncoderSim(const frc::DutyCycleEncoder& encoder) {
  frc::sim::SimDeviceSim deviceSim{"DutyCycle:DutyCycleEncoder",
                                   encoder.GetSourceChannel()};
  m_simPosition = deviceSim.GetDouble("position");
  m_simDistancePerRotation = deviceSim.GetDouble("distance_per_rot");
}

void DutyCycleEncoderSim::Set(units::turn_t turns) {
  m_simPosition.Set(turns.value());
}

void DutyCycleEncoderSim::SetDistance(double distance) {
  m_simPosition.Set(distance / m_simDistancePerRotation.Get());
}
