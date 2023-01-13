// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Intake.h"

frc2::CommandPtr Intake::IntakeCommand() {
  return RunOnce([this] { m_piston.Set(frc::DoubleSolenoid::kForward); })
      .AndThen(Run([this] { m_motor.Set(1.0); }))
      .WithName("Intake");
}

frc2::CommandPtr Intake::RetractCommand() {
  return RunOnce([this] {
           m_motor.Disable();
           m_piston.Set(frc::DoubleSolenoid::kReverse);
         })
      .WithName("Retract");
}
