// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/commands2/Command.hpp>
#include <wpi/commands2/CommandPtr.hpp>
#include <wpi/commands2/Commands.hpp>
#include <wpi/smartdashboard/SendableChooser.hpp>

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
  // The enum used as keys for selecting the command to run.
  enum CommandSelector { ONE, TWO, THREE };

  // An example of how command selector may be used with SendableChooser
  wpi::SendableChooser<CommandSelector> m_chooser;

  // The robot's subsystems and commands are defined here...

  // An example selectcommand.  Will select from the three commands based on the
  // value returned by the selector method at runtime.  Note that selectcommand
  // takes a generic type, so the selector does not have to be an enum; it could
  // be any desired type (string, integer, boolean, double...)
  wpi::cmd::CommandPtr m_exampleSelectCommand =
      wpi::cmd::cmd::Select<CommandSelector>(
          [this] { return m_chooser.GetSelected(); },
          // Maps selector values to commands
          std::pair{ONE, wpi::cmd::cmd::Print("Command one was selected!")},
          std::pair{TWO, wpi::cmd::cmd::Print("Command two was selected!")},
          std::pair{THREE,
                    wpi::cmd::cmd::Print("Command three was selected!")});

  void ConfigureButtonBindings();
};
