// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/smartdashboard/SendableChooser.hpp>
#include <wpi/commands2/Command.hpp>
#include <wpi/commands2/Commands.hpp>
#include <wpi/commands2/button/CommandPS4Controller.hpp>

#include "Constants.hpp"
#include "commands/Autos.hpp"
#include "subsystems/DriveSubsystem.hpp"
#include "subsystems/HatchSubsystem.hpp"

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

  frc2::Command* GetAutonomousCommand();

 private:
  // The driver's controller
  frc2::CommandPS4Controller m_driverController{
      OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;
  HatchSubsystem m_hatch;

  // Commands owned by RobotContainer

  // The autonomous routines
  frc2::CommandPtr m_simpleAuto = autos::SimpleAuto(&m_drive);
  frc2::CommandPtr m_complexAuto = autos::ComplexAuto(&m_drive, &m_hatch);

  // The chooser for the autonomous routines
  frc::SendableChooser<frc2::Command*> m_chooser;

  void ConfigureButtonBindings();
};
