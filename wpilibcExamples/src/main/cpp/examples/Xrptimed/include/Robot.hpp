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
  wpi::xrp::XRPMotor leftMotor{0};
  wpi::xrp::XRPMotor rightMotor{1};
  // Assumes a gamepad plugged into channel 0
  wpi::Joystick controller{0};
  wpi::Timer timer;

  wpi::DifferentialDrive drive{
      [&](double output) { leftMotor.SetThrottle(output); },
      [&](double output) { rightMotor.SetThrottle(output); }};
};
