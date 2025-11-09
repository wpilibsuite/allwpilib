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
  static constexpr int kAlliancePort = 0;
  static constexpr int kEnabledPort = 1;
  static constexpr int kAutonomousPort = 2;
  static constexpr int kAlertPort = 3;

  void RobotPeriodic() override;

 private:
  wpi::DigitalOutput m_allianceOutput{kAlliancePort};
  wpi::DigitalOutput m_enabledOutput{kEnabledPort};
  wpi::DigitalOutput m_autonomousOutput{kAutonomousPort};
  wpi::DigitalOutput m_alertOutput{kAlertPort};
};
