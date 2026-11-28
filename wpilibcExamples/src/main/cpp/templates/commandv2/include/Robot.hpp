// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>

#include "Constants.hpp"
#include "subsystems/ExampleSubsystem.hpp"
#include "wpi/commands2/CommandPtr.hpp"
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
  void SimulationInit() override;
  void SimulationPeriodic() override;

 private:
  void ConfigureBindings();
  // Have it empty by default so that if testing teleop it
  // doesn't have undefined behavior and potentially crash.
  std::optional<wpi::cmd::CommandPtr> autonomousCommand;

  // Replace with CommandPS4Controller or CommandJoystick if needed
  wpi::cmd::CommandGamepad driverController{
      OperatorConstants::DRIVER_CONTROLLER_PORT};

  // The robot's subsystems are defined here...
  ExampleSubsystem subsystem;
};
