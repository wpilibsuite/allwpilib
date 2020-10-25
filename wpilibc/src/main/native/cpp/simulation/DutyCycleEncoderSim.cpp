/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/DutyCycleEncoderSim.h"

#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/DutyCycleEncoder.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

DutyCycleEncoderSim::DutyCycleEncoderSim(const frc::DutyCycleEncoder& encoder) {
  wpi::SmallString<128> fullname;
  wpi::raw_svector_ostream os(fullname);
  os << "DutyCycleEncoder" << '[' << encoder.GetFPGAIndex() << ']';
  frc::sim::SimDeviceSim deviceSim{fullname.c_str()};
  m_simPosition = deviceSim.GetDouble("Position");
  m_simDistancePerRotation = deviceSim.GetDouble("DistancePerRotation");
}

void DutyCycleEncoderSim::Set(units::turn_t turns) {
  m_simPosition.Set(turns.to<double>());
}

void DutyCycleEncoderSim::SetDistance(double distance) {
  m_simPosition.Set(distance / m_simDistancePerRotation.Get());
}
