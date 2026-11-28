// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>

#include "Constants.hpp"
#include "subsystems/DriveSubsystem.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/button/CommandGamepad.hpp"
#include "wpi/framework/TimedRobot.hpp"

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
  std::optional<wpi::cmd::CommandPtr> autonomousCommand;

  // The driver's controller
  wpi::cmd::CommandGamepad driverController{
      OIConstants::DRIVER_CONTROLLER_PORT};

  // The robot's subsystems
  DriveSubsystem drive;

  // Robot-owned commands
  wpi::cmd::CommandPtr driveHalfVelocity =
      wpi::cmd::RunOnce([this] { drive.SetMaxOutput(0.5); }, {});
  wpi::cmd::CommandPtr driveFullVelocity =
      wpi::cmd::RunOnce([this] { drive.SetMaxOutput(1); }, {});
};
