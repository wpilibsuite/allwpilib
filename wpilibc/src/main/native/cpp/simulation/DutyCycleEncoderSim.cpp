// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DutyCycleEncoderSim.h"

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/DutyCycleEncoder.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

DutyCycleEncoderSim::DutyCycleEncoderSim(const frc::DutyCycleEncoder& encoder) {
  wpi::SmallString<128> fullname;
  wpi::raw_svector_ostream os(fullname);
  os << "DutyCycle:DutyCycleEncoder" << '[' << encoder.GetSourceChannel()
     << ']';
  frc::sim::SimDeviceSim deviceSim{fullname.c_str()};
  m_simPosition = deviceSim.GetDouble("position");
  m_simDistancePerRotation = deviceSim.GetDouble("distance_per_rot");
}

void DutyCycleEncoderSim::Set(units::turn_t turns) {
  m_simPosition.Set(turns.to<double>());
}

void DutyCycleEncoderSim::SetDistance(double distance) {
  m_simPosition.Set(distance / m_simDistancePerRotation.Get());
}
