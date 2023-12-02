// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/XboxController.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <frc2/command/Command.h>

#include "Constants.h"
#include "commands/ComplexAuto.h"
#include "commands/DefaultDrive.h"
#include "commands/DriveDistance.h"
#include "subsystems/DriveSubsystem.h"
#include "subsystems/HatchSubsystem.h"

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

  frc2::Command* GetAutonomousCommand();

 private:
  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;
  HatchSubsystem m_hatch;

  // The autonomous routines
  DriveDistance m_simpleAuto{AutoConstants::kAutoDriveDistanceInches,
                             AutoConstants::kAutoDriveSpeed, &m_drive};
  ComplexAuto m_complexAuto{&m_drive, &m_hatch};

  // The chooser for the autonomous routines
  frc::SendableChooser<frc2::Command*> m_chooser;

  // The driver's controller
  frc::XboxController m_driverController{OIConstants::kDriverControllerPort};

  void ConfigureButtonBindings();
};
