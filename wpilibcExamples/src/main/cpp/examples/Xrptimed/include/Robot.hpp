// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/xrp/XRPMotor.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot();
  void RobotPeriodic() override;
  void AutonomousInit() override;
  void AutonomousPeriodic() override;
  void TeleopInit() override;
  void TeleopPeriodic() override;

 private:
  wpi::xrp::XRPMotor m_leftMotor{0};
  wpi::xrp::XRPMotor m_rightMotor{1};
  // Assumes a gamepad plugged into channel 0
  wpi::Joystick m_controller{0};
  wpi::Timer m_timer;

  wpi::DifferentialDrive m_drive{
      [&](double output) { m_leftMotor.SetDutyCycle(output); },
      [&](double output) { m_rightMotor.SetDutyCycle(output); }};
};
