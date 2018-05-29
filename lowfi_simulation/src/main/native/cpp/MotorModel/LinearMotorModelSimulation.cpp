/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MotorModel/LinearMotorModelSimulation.h"

namespace frc {
namespace sim {
namespace lowfi {

LinearMotorModelSimulation::LinearMotorModelSimulation(double maxSpeed)
    : m_maxSpeed(maxSpeed) {}

void LinearMotorModelSimulation::Reset() {
  m_position = 0;
  m_velocity = 0;
}

void LinearMotorModelSimulation::SetVoltage(double voltage) {
  m_voltagePercentage = voltage / kMaxExpectedVoltage;
}

void LinearMotorModelSimulation::Update(double elapsedTime) {
  m_velocity = m_maxSpeed * m_voltagePercentage;
  m_position += m_velocity * elapsedTime;
}

double LinearMotorModelSimulation::GetPosition() const { return m_position; }

double LinearMotorModelSimulation::GetVelocity() const { return m_velocity; }

double LinearMotorModelSimulation::GetAcceleration() const { return 0; }

double LinearMotorModelSimulation::GetCurrent() const { return 0; }

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
