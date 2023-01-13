// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/DigitalInput.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/CommandPtr.h>
#include <frc2/command/SubsystemBase.h>

#include "Constants.h"

class Storage : frc2::SubsystemBase {
 public:
  Storage();
  /** Returns a command that runs the storage motor indefinitely. */
  [[nodiscard]] frc2::CommandPtr RunCommand();

  /** Whether the ball storage is full. */
  bool IsFull() const;

 private:
  frc::PWMSparkMax m_motor{StorageConstants::kMotorPort};
  frc::DigitalInput m_ballSensor{StorageConstants::kBallSensorPort};
};
