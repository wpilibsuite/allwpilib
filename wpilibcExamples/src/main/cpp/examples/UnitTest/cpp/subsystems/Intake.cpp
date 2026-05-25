// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Intake.hpp"

void Intake::Deploy() {
  piston.Set(wpi::DoubleSolenoid::Value::FORWARD);
}

void Intake::Retract() {
  piston.Set(wpi::DoubleSolenoid::Value::REVERSE);
  motor.SetThrottle(0);  // turn off the motor
}

void Intake::Activate(double velocity) {
  if (IsDeployed()) {
    motor.SetThrottle(velocity);
  } else {  // if piston isn't open, do nothing
    motor.SetThrottle(0);
  }
}

bool Intake::IsDeployed() const {
  return piston.Get() == wpi::DoubleSolenoid::Value::FORWARD;
}
