// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/smartdashboard/SmartDashboard.h>
#include <frc2/command/Commands.h>
#include <frc2/command/button/JoystickButton.h>

#include "commands/TeleopArcadeDrive.h"

RobotContainer::RobotContainer() {
  // Configure the button bindings
  ConfigureButtonBindings();
}

void RobotContainer::ConfigureButtonBindings() {
  // Also set default commands here
  m_drive.SetDefaultCommand(TeleopArcadeDrive(
      &m_drive, [this] { return -m_controller.GetRawAxis(1); },
      [this] { return -m_controller.GetRawAxis(2); }));

  // Example of how to use the onboard IO
  m_userButton.OnTrue(frc2::cmd::Print("USER Button Pressed"))
      .OnFalse(frc2::cmd::Print("USER Button Released"));

  frc2::JoystickButton(&m_controller, 1)
      .OnTrue(frc2::cmd::RunOnce([this] { m_arm.SetAngle(45_deg); }, {}))
      .OnFalse(frc2::cmd::RunOnce([this] { m_arm.SetAngle(0_deg); }, {}));

  frc2::JoystickButton(&m_controller, 2)
      .OnTrue(frc2::cmd::RunOnce([this] { m_arm.SetAngle(90_deg); }, {}))
      .OnFalse(frc2::cmd::RunOnce([this] { m_arm.SetAngle(0_deg); }, {}));

  // Setup SmartDashboard options.
  m_chooser.SetDefaultOption("Auto Routine Distance", &m_autoDistance);
  m_chooser.AddOption("Auto Routine Time", &m_autoTime);
  frc::SmartDashboard::PutData("Auto Selector", &m_chooser);
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  return m_chooser.GetSelected();
}
