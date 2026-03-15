// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Intake.hpp"

void Intake::Deploy() {
  m_piston.Set(wpi::DoubleSolenoid::Value::kForward);
}

void Intake::Retract() {
  m_piston.Set(wpi::DoubleSolenoid::Value::kReverse);
  m_motor.SetDutyCycle(0);  // turn off the motor
}

void Intake::Activate(double velocity) {
  if (IsDeployed()) {
    m_motor.SetDutyCycle(velocity);
  } else {  // if piston isn't open, do nothing
    m_motor.SetDutyCycle(0);
  }
}

bool Intake::IsDeployed() const {
  return m_piston.Get() == wpi::DoubleSolenoid::Value::kForward;
}
