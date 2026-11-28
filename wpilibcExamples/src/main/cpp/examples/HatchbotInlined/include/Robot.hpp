// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "commands/Autos.hpp"
#include "subsystems/DriveSubsystem.hpp"
#include "subsystems/HatchSubsystem.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/button/CommandGamepad.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/tunables/Selectable.hpp"

namespace ac = AutoConstants;

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
  // Have it null by default so that if testing teleop it
  // doesn't have undefined behavior and potentially crash.
  wpi::cmd::Command* autonomousCommand = nullptr;

  // The driver's controller
  wpi::cmd::CommandGamepad driverController{
      OIConstants::DRIVER_CONTROLLER_PORT};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem drive;
  HatchSubsystem hatch;

  // Commands owned by Robot

  // The autonomous routines
  wpi::cmd::CommandPtr simpleAuto = autos::SimpleAuto(&drive);
  wpi::cmd::CommandPtr complexAuto = autos::ComplexAuto(&drive, &hatch);

  // The chooser for the autonomous routines
  wpi::tunables::Selectable<wpi::cmd::Command*> chooser;
};
