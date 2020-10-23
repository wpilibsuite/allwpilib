/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/AnalogEncoderSim.h"

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/AnalogEncoder.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

AnalogEncoderSim::AnalogEncoderSim(const frc::AnalogEncoder& encoder) {
  wpi::SmallString<128> fullname;
  wpi::raw_svector_ostream os(fullname);
  os << "AnalogEncoder" << '[' << encoder.GetChannel() << ']';
  frc::sim::SimDeviceSim deviceSim{fullname.c_str()};
  m_positionSim = deviceSim.GetDouble("Position");
}

void AnalogEncoderSim::SetPosition(frc::Rotation2d angle) {
  SetTurns(angle.Degrees());
}

void AnalogEncoderSim::SetTurns(units::turn_t turns) {
  m_positionSim.Set(turns.to<double>());
}

units::turn_t AnalogEncoderSim::GetTurns() {
  return units::turn_t{m_positionSim.Get()};
}

frc::Rotation2d AnalogEncoderSim::GetPosition() {
  units::radian_t rads = GetTurns();
  return frc::Rotation2d{rads};
}
