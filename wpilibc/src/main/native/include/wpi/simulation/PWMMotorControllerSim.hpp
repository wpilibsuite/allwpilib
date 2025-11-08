// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/SimDevice.h"
#include "wpi/hardware/motor/PWMMotorController.hpp"
#include "wpi/units/length.hpp"

namespace frc {

class PWMMotorController;

namespace sim {

class PWMMotorControllerSim {
 public:
  explicit PWMMotorControllerSim(const PWMMotorController& motorctrl);

  explicit PWMMotorControllerSim(int channel);

  double GetSpeed() const;

 private:
  hal::SimDouble m_simSpeed;
};
}  // namespace sim
}  // namespace frc
