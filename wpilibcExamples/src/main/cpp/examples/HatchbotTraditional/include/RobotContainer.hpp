// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "commands/ComplexAuto.hpp"
#include "commands/DefaultDrive.hpp"
#include "commands/DriveDistance.hpp"
#include "subsystems/DriveSubsystem.hpp"
#include "subsystems/HatchSubsystem.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/smartdashboard/SendableChooser.hpp"

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

  wpi::cmd::Command* GetAutonomousCommand();

 private:
  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem drive;
  HatchSubsystem hatch;

  // The autonomous routines
  DriveDistance simpleAuto{AutoConstants::kAutoDriveDistanceInches,
                           AutoConstants::kAutoDriveVelocity, &drive};
  ComplexAuto complexAuto{&drive, &hatch};

  // The chooser for the autonomous routines
  wpi::SendableChooser<wpi::cmd::Command*> chooser;

  // The driver's controller
  wpi::Gamepad driverController{OIConstants::kDriverControllerPort};

  void ConfigureButtonBindings();
};
