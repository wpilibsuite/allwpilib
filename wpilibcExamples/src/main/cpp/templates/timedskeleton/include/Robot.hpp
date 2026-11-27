// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/framework/TimedRobot.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot();
  void RobotPeriodic() override;

  void AutonomousStart() override;
  void AutonomousPeriodic() override;
  void AutonomousEnd() override;

  void TeleopStart() override;
  void TeleopPeriodic() override;
  void TeleopEnd() override;

  void DisabledStart() override;
  void DisabledPeriodic() override;
  void DisabledEnd() override;

  void UtilityStart() override;
  void UtilityPeriodic() override;
  void UtilityEnd() override;

  void SimulationStart() override;
  void SimulationPeriodic() override;
};
