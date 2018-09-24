/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "lowfisim/wpisimulators/WpiMotorSim.h"

namespace frc {
namespace sim {
namespace lowfi {

WpiMotorSim::WpiMotorSim(int index, MotorModel& motorModelSimulator)
    : m_motorModelSimulation(motorModelSimulator), m_pwmSimulator(index) {}

void WpiMotorSim::Update(double elapsedTime) {
  m_motorModelSimulation.SetVoltage(m_pwmSimulator.GetSpeed() *
                                    kDefaultVoltage);
  m_motorModelSimulation.Update(elapsedTime);
}

bool WpiMotorSim::IsWrapperInitialized() const {
  return m_pwmSimulator.GetInitialized();
}

double WpiMotorSim::GetPosition() const {
  return m_motorModelSimulation.GetPosition();
}

double WpiMotorSim::GetVelocity() const {
  return m_motorModelSimulation.GetVelocity();
}

double WpiMotorSim::GetAcceleration() const {
  return m_motorModelSimulation.GetAcceleration();
}

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
