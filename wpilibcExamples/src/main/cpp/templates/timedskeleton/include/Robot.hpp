// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/framework/TimedRobot.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot();
  void RobotPeriodic() override;

  void AutonomousInit() override;
  void AutonomousPeriodic() override;
  void AutonomousExit() override;

  void TeleopInit() override;
  void TeleopPeriodic() override;
  void TeleopExit() override;

  void DisabledInit() override;
  void DisabledPeriodic() override;
  void DisabledExit() override;

  void UtilityInit() override;
  void UtilityPeriodic() override;
  void UtilityExit() override;

  void SimulationInit() override;
  void SimulationPeriodic() override;
};
