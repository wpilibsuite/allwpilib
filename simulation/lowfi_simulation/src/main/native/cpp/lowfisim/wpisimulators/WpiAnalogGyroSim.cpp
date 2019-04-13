/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "lowfisim/wpisimulators/WpiAnalogGyroSim.h"

namespace frc {
namespace sim {
namespace lowfi {

WpiAnalogGyroSim::WpiAnalogGyroSim(int index) : m_gyroSimulator(index) {}

bool WpiAnalogGyroSim::IsWrapperInitialized() const {
  return m_gyroSimulator.GetInitialized();
}

void WpiAnalogGyroSim::SetAngle(double angle) {
  m_gyroSimulator.SetAngle(angle);
}

double WpiAnalogGyroSim::GetAngle() { return m_gyroSimulator.GetAngle(); }

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
