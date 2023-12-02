// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/Command.h>
#include <frc2/command/CommandPtr.h>
#include <frc2/command/Commands.h>
#include <frc2/command/button/CommandXboxController.h>

#include "Constants.h"
#include "subsystems/DriveSubsystem.h"
#include "subsystems/ShooterSubsystem.h"

namespace ac = AutoConstants;

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

  // The chooser for the autonomous routines
  frc2::CommandPtr GetAutonomousCommand();

 private:
  // The driver's controller
  frc2::CommandXboxController m_driverController{
      OIConstants::kDriverControllerPort};

  // The robot's subsystems
  DriveSubsystem m_drive;
  ShooterSubsystem m_shooter;

  // RobotContainer-owned commands
  // (These variables will still be valid after binding, because we don't move
  // ownership)

  frc2::CommandPtr m_spinUpShooter =
      frc2::cmd::RunOnce([this] { m_shooter.Enable(); }, {&m_shooter});

  frc2::CommandPtr m_stopShooter =
      frc2::cmd::RunOnce([this] { m_shooter.Disable(); }, {&m_shooter});

  void ConfigureButtonBindings();
};
