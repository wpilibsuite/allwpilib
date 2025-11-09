// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

Robot::Robot() {
  // Default to a length of 60, start empty output
  m_led.SetLength(kLength);
  m_led.SetData(m_ledBuffer);
}

void Robot::RobotPeriodic() {
  // Run the rainbow pattern and apply it to the buffer
  m_scrollingRainbow.ApplyTo(m_ledBuffer);
  // Set the LEDs
  m_led.SetData(m_ledBuffer);
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
