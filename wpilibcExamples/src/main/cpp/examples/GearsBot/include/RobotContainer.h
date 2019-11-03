/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/Joystick.h>
#include <frc2/command/Command.h>

#include "commands/Autonomous.h"
#include "subsystems/Claw.h"
#include "subsystems/DriveTrain.h"
#include "subsystems/Elevator.h"
#include "subsystems/Wrist.h"

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
  frc::Joystick m_joy{0};

  Claw m_claw;
  Wrist m_wrist;
  Elevator m_elevator;
  DriveTrain m_drivetrain;

  Autonomous m_autonomousCommand;

  void ConfigureButtonBindings();
};
