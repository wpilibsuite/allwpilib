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
  // Have it null by default so that if testing teleop it
  // doesn't have undefined behavior and potentially crash.
  wpi::cmd::Command* autonomousCommand = nullptr;

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem drive;
  HatchSubsystem hatch;

  // The autonomous routines
  DriveDistance simpleAuto{AutoConstants::AUTO_DRIVE_DISTANCE_INCHES,
                           AutoConstants::AUTO_DRIVE_VELOCITY, &drive};
  ComplexAuto complexAuto{&drive, &hatch};

  // The chooser for the autonomous routines
  wpi::tunables::Selectable<wpi::cmd::Command*> chooser;

  // The driver's controller
  wpi::Gamepad driverController{OIConstants::DRIVER_CONTROLLER_PORT};
};
