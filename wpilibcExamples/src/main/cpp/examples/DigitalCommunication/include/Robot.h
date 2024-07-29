// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <frc/DigitalOutput.h>
#include <frc/TimedRobot.h>

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's DIO ports.
 */
class Robot : public frc::TimedRobot {
 public:
  // define ports for communication with light controller
  static constexpr int kAlliancePort = 0;
  static constexpr int kEnabledPort = 1;
  static constexpr int kAutonomousPort = 2;
  static constexpr int kAlertPort = 3;

  void RobotPeriodic() override;

 private:
  frc::DigitalOutput m_allianceOutput{kAlliancePort};
  frc::DigitalOutput m_enabledOutput{kEnabledPort};
  frc::DigitalOutput m_autonomousOutput{kAutonomousPort};
  frc::DigitalOutput m_alertOutput{kAlertPort};
};
