// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/tunables/Selectable.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot();
  void RobotPeriodic() override;
  void DisabledStart() override;
  void DisabledPeriodic() override;
  void AutonomousStart() override;
  void AutonomousPeriodic() override;
  void AutonomousEnd() override;
  void TeleopStart() override;
  void TeleopPeriodic() override;
  void UtilityPeriodic() override;

 private:
  void ConfigureButtonBindings();
  // An example selectcommand.  Will select from the three commands based on the
  // value returned by the selector method at runtime.  Note that selectcommand
  // takes a generic type, so the selector does not have to be an enum; it could
  // be any desired type (string, integer, boolean, double...)
  wpi::cmd::CommandPtr autonomousCommand = wpi::cmd::Select<CommandSelector>(
      [this] { return chooser.GetSelected(); },
      // Maps selector values to commands
      std::pair{ONE, wpi::cmd::Print("Command one was selected!")},
      std::pair{TWO, wpi::cmd::Print("Command two was selected!")},
      std::pair{THREE, wpi::cmd::Print("Command three was selected!")});

  // The enum used as keys for selecting the command to run.
  enum CommandSelector { ONE, TWO, THREE };

  // An example of how command selector may be used with
  // wpi::tunables::Selectable
  wpi::tunables::Selectable<CommandSelector> chooser;
};
