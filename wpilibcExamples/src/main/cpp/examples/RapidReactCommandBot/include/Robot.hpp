// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>

#include "RapidReactCommandBot.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/framework/TimedRobot.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot();
  void RobotPeriodic() override;
  void DisabledEnter() override;
  void DisabledPeriodic() override;
  void AutonomousEnter() override;
  void AutonomousPeriodic() override;
  void AutonomousExit() override;
  void TeleopEnter() override;
  void TeleopPeriodic() override;
  void UtilityEnter() override;
  void UtilityPeriodic() override;

 private:
  RapidReactCommandBot robot;
  std::optional<wpi::cmd::CommandPtr> autonomousCommand;
};
