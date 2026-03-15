// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Intake.hpp"

wpi::cmd::CommandPtr Intake::IntakeCommand() {
  return RunOnce([this] { m_piston.Set(wpi::DoubleSolenoid::kForward); })
      .AndThen(Run([this] { m_motor.SetDutyCycle(1.0); }))
      .WithName("Intake");
}

wpi::cmd::CommandPtr Intake::RetractCommand() {
  return RunOnce([this] {
           m_motor.Disable();
           m_piston.Set(wpi::DoubleSolenoid::kReverse);
         })
      .WithName("Retract");
}
