// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <frc/I2C.h>
#include <frc/TimedRobot.h>

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's I2C port.
 */
class Robot : public frc::TimedRobot {
 public:
  void RobotPeriodic() override;

  static constexpr frc::I2C::Port PORT = frc::I2C::Port::PORT0;

 private:
  static constexpr int deviceAddress = 4;
  frc::I2C arduino{PORT, deviceAddress};
};
