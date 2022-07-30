// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/XboxController.h>
#include <frc/command/Command.h>
#include <frc/command/InstantCommand.h>
#include <frc/command/WaitCommand.h>

#include "Constants.h"

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
  // The robot's subsystems and commands are defined here...

  // The driver's controller
  frc::XboxController m_driverController{OIConstants::kDriverControllerPort};

  // Three commands that do nothing; for demonstration purposes.
  frc::InstantCommand m_instantCommand1;
  frc::InstantCommand m_instantCommand2;
  frc::WaitCommand m_waitCommand{5_s};

  void ConfigureButtonBindings();
};
