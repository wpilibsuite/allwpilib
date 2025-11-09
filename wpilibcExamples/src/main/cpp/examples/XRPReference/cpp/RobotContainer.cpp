// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "commands/TeleopArcadeDrive.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/button/JoystickButton.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

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
  m_userButton.OnTrue(wpi::cmd::cmd::Print("USER Button Pressed"))
      .OnFalse(wpi::cmd::cmd::Print("USER Button Released"));

  wpi::cmd::JoystickButton(&m_controller, 1)
      .OnTrue(wpi::cmd::cmd::RunOnce([this] { m_arm.SetAngle(45_deg); }, {}))
      .OnFalse(wpi::cmd::cmd::RunOnce([this] { m_arm.SetAngle(0_deg); }, {}));

  wpi::cmd::JoystickButton(&m_controller, 2)
      .OnTrue(wpi::cmd::cmd::RunOnce([this] { m_arm.SetAngle(90_deg); }, {}))
      .OnFalse(wpi::cmd::cmd::RunOnce([this] { m_arm.SetAngle(0_deg); }, {}));

  // Setup SmartDashboard options.
  m_chooser.SetDefaultOption("Auto Routine Distance", &m_autoDistance);
  m_chooser.AddOption("Auto Routine Time", &m_autoTime);
  wpi::SmartDashboard::PutData("Auto Selector", &m_chooser);
}

wpi::cmd::Command* RobotContainer::GetAutonomousCommand() {
  return m_chooser.GetSelected();
}
