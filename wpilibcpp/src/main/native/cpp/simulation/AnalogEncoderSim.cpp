// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogEncoderSim.h"

#include "frc/AnalogEncoder.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

AnalogEncoderSim::AnalogEncoderSim(const frc::AnalogEncoder& encoder) {
  frc::sim::SimDeviceSim deviceSim{"AnalogEncoder", encoder.GetChannel()};
  m_positionSim = deviceSim.GetDouble("Position");
}

void AnalogEncoderSim::SetPosition(frc::Rotation2d angle) {
  SetTurns(angle.Degrees());
}

void AnalogEncoderSim::SetTurns(units::turn_t turns) {
  m_positionSim.Set(turns.value());
}

units::turn_t AnalogEncoderSim::GetTurns() {
  return units::turn_t{m_positionSim.Get()};
}

frc::Rotation2d AnalogEncoderSim::GetPosition() {
  return units::radian_t{GetTurns()};
}
