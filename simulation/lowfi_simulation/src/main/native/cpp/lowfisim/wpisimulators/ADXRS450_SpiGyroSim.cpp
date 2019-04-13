/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "lowfisim/wpisimulators/ADXRS450_SpiGyroSim.h"
#pragma warning(disable: 4244 4267 4146)

namespace frc {
namespace sim {
namespace lowfi {

ADXRS450_SpiGyroSim::ADXRS450_SpiGyroSim(int spiPort)
    : m_gyroWrapper(spiPort) {}

bool ADXRS450_SpiGyroSim::IsWrapperInitialized() const {
  return m_gyroWrapper.GetInitialized();
}

void ADXRS450_SpiGyroSim::SetAngle(double angle) {
  m_gyroWrapper.SetAngle(angle);
}

double ADXRS450_SpiGyroSim::GetAngle() { return m_gyroWrapper.GetAngle(); }

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
