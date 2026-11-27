// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/tunables/Selectable.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot();
  void RobotPeriodic() override;
  void AutonomousStart() override;
  void AutonomousPeriodic() override;
  void TeleopStart() override;
  void TeleopPeriodic() override;
  void DisabledStart() override;
  void DisabledPeriodic() override;
  void UtilityStart() override;
  void UtilityPeriodic() override;
  void SimulationStart() override;
  void SimulationPeriodic() override;

 private:
  wpi::tunables::Selectable<std::string> chooser;
  const std::string AUTO_NAME_DEFAULT = "Default";
  const std::string AUTO_NAME_CUSTOM = "My Auto";
  std::string autoSelected;
};
