// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "subsystems/DriveSubsystem.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/button/CommandXboxController.hpp"

/**
 * This class is where the bulk of the robot should be declared.  Since
 * Command-based is a "declarative" paradigm, very little robot logic should
 * actually be handled in the {@link Robot} periodic methods (other than the
 * scheduler calls).  Instead, the structure of the robot (including subsystems,
 * commands, and button mappings) should be declared here.
 */
class RobotContainer {
 public:
  RobotContainer();

  wpi::cmd::CommandPtr GetAutonomousCommand();

 private:
  // The driver's controller
  wpi::cmd::CommandXboxController m_driverController{
      OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;

  // RobotContainer-owned commands
  wpi::cmd::CommandPtr m_driveHalfSpeed =
      wpi::cmd::cmd::RunOnce([this] { m_drive.SetMaxOutput(0.5); }, {});
  wpi::cmd::CommandPtr m_driveFullSpeed =
      wpi::cmd::cmd::RunOnce([this] { m_drive.SetMaxOutput(1); }, {});

  void ConfigureButtonBindings();
};
