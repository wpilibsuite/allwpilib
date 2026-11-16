// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/discrete/DigitalOutput.hpp"

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's DIO ports.
 */
class Robot : public wpi::TimedRobot {
 public:
  // define ports for communication with light controller
  static constexpr int ALLIANCE_PORT = 0;
  static constexpr int ENABLED_PORT = 1;
  static constexpr int AUTONOMOUS_PORT = 2;
  static constexpr int ALERT_PORT = 3;

  void RobotPeriodic() override;

 private:
  wpi::DigitalOutput allianceOutput{ALLIANCE_PORT};
  wpi::DigitalOutput enabledOutput{ENABLED_PORT};
  wpi::DigitalOutput autonomousOutput{AUTONOMOUS_PORT};
  wpi::DigitalOutput alertOutput{ALERT_PORT};
};
