// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/framework/TimesliceRobot.hpp"
#include "wpi/tunables/Selectable.hpp"

class Robot : public wpi::TimesliceRobot {
 public:
  Robot();
  void RobotPeriodic() override;
  void AutonomousInit() override;
  void AutonomousPeriodic() override;
  void TeleopInit() override;
  void TeleopPeriodic() override;
  void DisabledInit() override;
  void DisabledPeriodic() override;
  void UtilityInit() override;
  void UtilityPeriodic() override;

 private:
  wpi::tunables::Selectable<std::string> chooser;
  const std::string AUTO_NAME_DEFAULT = "Default";
  const std::string AUTO_NAME_CUSTOM = "My Auto";
  std::string autoSelected;
};
