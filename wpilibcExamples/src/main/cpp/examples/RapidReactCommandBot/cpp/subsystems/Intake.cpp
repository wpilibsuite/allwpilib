// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Intake.hpp"

wpi::cmd::CommandPtr Intake::IntakeCommand() {
  return RunOnce([this] { piston.Set(wpi::DoubleSolenoid::FORWARD); })
      .AndThen(Run([this] { motor.SetThrottle(1.0); }))
      .WithName("Intake");
}

wpi::cmd::CommandPtr Intake::RetractCommand() {
  return RunOnce([this] {
           motor.Disable();
           piston.Set(wpi::DoubleSolenoid::REVERSE);
         })
      .WithName("Retract");
}
