// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sim/IntakeSim.h"


void IntakeSim::SimulationPeriodic() {}

double IntakeSim::GetMotor() const {
  return m_motor.GetSpeed();
}

bool IntakeSim::IsDeployed() const {
  return m_piston.Get() == frc::DoubleSolenoid::Value::kForward;
}
