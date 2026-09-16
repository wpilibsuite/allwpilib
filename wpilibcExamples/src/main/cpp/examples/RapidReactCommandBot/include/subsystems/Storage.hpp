// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/hardware/discrete/DigitalInput.hpp"

class Storage : wpi::cmd::SubsystemBase {
 public:
  Storage();
  /** Returns a command that runs the storage motor indefinitely. */
  wpi::cmd::CommandPtr RunCommand();

  /** Whether the ball storage is full. */
  wpi::cmd::Trigger HasCargo{[this] { return ballSensor.Get(); }};

 private:
  wpi::PWMSparkMax motor{StorageConstants::MOTOR_PORT};
  wpi::DigitalInput ballSensor{StorageConstants::BALL_SENSOR_PORT};
};
