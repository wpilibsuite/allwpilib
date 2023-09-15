// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command2/Command.h>
#include <frc/command2/CommandPtr.h>
#include <frc/command2/Commands.h>
#include <frc/command2/button/CommandXboxController.h>

#include "Constants.h"
#include "subsystems/DriveSubsystem.h"

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

  frc::Command* GetAutonomousCommand();

 private:
  // The driver's controller
  frc::CommandXboxController m_driverController{
      OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;

  // RobotContainer-owned commands
  frc::CommandPtr m_driveHalfSpeed =
      frc::cmd::RunOnce([this] { m_drive.SetMaxOutput(0.5); }, {});
  frc::CommandPtr m_driveFullSpeed =
      frc::cmd::RunOnce([this] { m_drive.SetMaxOutput(1); }, {});

  void ConfigureButtonBindings();
};
