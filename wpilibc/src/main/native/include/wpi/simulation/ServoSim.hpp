// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <units/length.h>

#include "frc/Servo.h"

namespace frc {

class Servo;

namespace sim {
class ServoSim {
 public:
  explicit ServoSim(const Servo& servo);

  explicit ServoSim(int channel);

  double GetPosition() const;

  double GetAngle() const;

 private:
  hal::SimDouble m_simPosition;
};
}  // namespace sim
}  // namespace frc
