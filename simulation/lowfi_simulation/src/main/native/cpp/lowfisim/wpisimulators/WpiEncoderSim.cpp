/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "lowfisim/wpisimulators/WpiEncoderSim.h"

namespace frc {
namespace sim {
namespace lowfi {

WpiEncoderSim::WpiEncoderSim(int index) : m_encoderSimulator(index) {}

bool WpiEncoderSim::IsWrapperInitialized() const {
  return m_encoderSimulator.GetInitialized();
}

void WpiEncoderSim::SetPosition(double position) {
  m_encoderSimulator.SetCount(
      static_cast<int>(position / m_encoderSimulator.GetDistancePerPulse()));
}

void WpiEncoderSim::SetVelocity(double velocity) {
  m_encoderSimulator.SetPeriod(1.0 / velocity);
}

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
