// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command2/Command.h>
#include <frc/command2/Commands.h>
#include <frc/command2/button/CommandPS4Controller.h>
#include <frc/smartdashboard/SendableChooser.h>

#include "Constants.h"
#include "commands/Autos.h"
#include "subsystems/DriveSubsystem.h"
#include "subsystems/HatchSubsystem.h"

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
  frc::CommandPS4Controller m_driverController{
      OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;
  HatchSubsystem m_hatch;

  // Commands owned by RobotContainer

  // The autonomous routines
  frc::CommandPtr m_simpleAuto = autos::SimpleAuto(&m_drive);
  frc::CommandPtr m_complexAuto = autos::ComplexAuto(&m_drive, &m_hatch);

  // The chooser for the autonomous routines
  frc::SendableChooser<frc::Command*> m_chooser;

  void ConfigureButtonBindings();
};
