// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <utility>

#include "commands/ExampleCommand.h"

Robot::Robot() {
  // Schedule `ExampleCommand` when `exampleCondition` changes to `true`
  frc2::Trigger([this] {
    return m_subsystem.ExampleCondition();
  }).OnTrue(ExampleCommand(&m_subsystem).ToPtr());

  // Schedule `ExampleMethodCommand` when the Xbox controller's B button is
  // pressed, cancelling on release.
  m_driverController.B().WhileTrue(m_subsystem.ExampleMethodCommand());

  m_autoChooser.SetDefaultOption(
      "Example Auto",
      EXAMPLE_AUTO);  // An example Command that will be run in auto.
  m_autonomous.WhileTrue(frc2::cmd::DeferredProxy([this] {
    return frc2::cmd::Select<Autos>(
        [this] { return m_autoChooser.GetSelected(); },
        std::pair{EXAMPLE_AUTO, std::move(m_exampleAuto)});
  }));
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
