/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WpiSimulators/WpiEncoderSimulator.h"

namespace frc {
namespace sim {
namespace lowfi {

WpiEncoderSimulator::WpiEncoderSimulator(int index)
    : m_encoderSimulator(index) {}

WpiEncoderSimulator::~WpiEncoderSimulator() {}

void WpiEncoderSimulator::SetPosition(double position) {
  m_encoderSimulator.SetCount(
      static_cast<int>(position / m_encoderSimulator.GetDistancePerPulse()));
}

void WpiEncoderSimulator::SetVelocity(double velocity) {
  m_encoderSimulator.SetPeriod(1.0 / velocity);
}

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
