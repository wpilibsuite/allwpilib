// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>

#include "Constants.hpp"
#include "subsystems/Drive.hpp"
#include "subsystems/Intake.hpp"
#include "subsystems/Pneumatics.hpp"
#include "subsystems/Shooter.hpp"
#include "subsystems/Storage.hpp"
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
  void UtilityStart() override;
  void UtilityPeriodic() override;

 private:
  /**
   * Use this method to define bindings between conditions and commands. These
   * are useful for automating robot behaviors based on button and sensor input.
   *
   * <p>Should be called in the robot class constructor.
   *
   * <p>Event binding methods are available on the wpi::cmd::Trigger class.
   */
  void ConfigureBindings();
  // The robot's subsystems
  Drive drive;
  Intake intake;
  Shooter shooter;
  Storage storage;
  Pneumatics pneumatics;

  // The driver's controller
  wpi::cmd::CommandGamepad driverController{
      OIConstants::DRIVER_CONTROLLER_PORT};
  std::optional<wpi::cmd::CommandPtr> autonomousCommand;
};
