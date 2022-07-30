// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/XboxController.h>
#include <frc/command/Command.h>
#include <frc/command/ConditionalCommand.h>
#include <frc/command/InstantCommand.h>
#include <frc/command/ParallelRaceGroup.h>
#include <frc/command/RunCommand.h>
#include <frc/command/SequentialCommandGroup.h>
#include <frc/command/WaitCommand.h>
#include <frc/command/WaitUntilCommand.h>
#include <frc/smartdashboard/SendableChooser.h>

#include "Constants.h"
#include "subsystems/ArmSubsystem.h"
#include "subsystems/DriveSubsystem.h"

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

  frc::Command* GetAutonomousCommand();

 private:
  // The driver's controller
  frc::XboxController m_driverController{OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;
  ArmSubsystem m_arm;

  // The chooser for the autonomous routines

  void ConfigureButtonBindings();
};
