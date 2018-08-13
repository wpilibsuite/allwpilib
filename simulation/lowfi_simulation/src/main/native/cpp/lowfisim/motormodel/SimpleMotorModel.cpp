/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "lowfisim/motormodel/SimpleMotorModel.h"

namespace frc {
namespace sim {
namespace lowfi {

SimpleMotorModel::SimpleMotorModel(double maxSpeed) : m_maxSpeed(maxSpeed) {}

void SimpleMotorModel::Reset() {
  m_position = 0;
  m_velocity = 0;
}

void SimpleMotorModel::SetVoltage(double voltage) {
  m_voltagePercentage = voltage / kMaxExpectedVoltage;
}

void SimpleMotorModel::Update(double elapsedTime) {
  m_velocity = m_maxSpeed * m_voltagePercentage;
  m_position += m_velocity * elapsedTime;
}

double SimpleMotorModel::GetPosition() const { return m_position; }

double SimpleMotorModel::GetVelocity() const { return m_velocity; }

double SimpleMotorModel::GetAcceleration() const { return 0; }

double SimpleMotorModel::GetCurrent() const { return 0; }

}  // namespace lowfi
}  // namespace sim
}  // namespace frc
