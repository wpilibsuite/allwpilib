// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/TimedRobot.h>
#include <frc/Ultrasonic.h>

/**
 * This is a sample program demonstrating how to read from a ping-response
 * ultrasonic sensor with the {@link Ultrasonic class}.
 */
class Robot : public frc::TimedRobot {
 public:
  Robot();
  void TeleopPeriodic() override;
  void TestInit() override;
  void TestPeriodic() override;
  void TestExit() override;

 private:
  // Creates a ping-response Ultrasonic object on DIO 1 and 2.
  frc::Ultrasonic m_rangeFinder{1, 2};
};
