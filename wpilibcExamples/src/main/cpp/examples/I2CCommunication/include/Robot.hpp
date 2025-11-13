// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/bus/I2C.hpp"

/**
 * This is a sample program demonstrating how to communicate to a light
 * controller from the robot code using the roboRIO's I2C port.
 */
class Robot : public wpi::TimedRobot {
 public:
  void RobotPeriodic() override;

  static constexpr wpi::I2C::Port kPort = wpi::I2C::Port::kPort0;

 private:
  static constexpr int deviceAddress = 4;
  wpi::I2C arduino{kPort, deviceAddress};
};
