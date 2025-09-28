// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/command/CommandPtr.hpp>
#include <wpi/command/SubsystemBase.hpp>
#include <wpi/command/button/Trigger.hpp>
#include <wpi/hardware/discrete/DigitalInput.hpp>
#include <wpi/hardware/motor/PWMSparkMax.hpp>

#include "Constants.h"

class Storage : wpi::cmd::SubsystemBase {
 public:
  Storage();
  /** Returns a command that runs the storage motor indefinitely. */
  wpi::cmd::CommandPtr RunCommand();

  /** Whether the ball storage is full. */
  wpi::cmd::Trigger HasCargo{[this] { return m_ballSensor.Get(); }};

 private:
  wpi::PWMSparkMax m_motor{StorageConstants::kMotorPort};
  wpi::DigitalInput m_ballSensor{StorageConstants::kBallSensorPort};
};
