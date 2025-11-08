// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/PWMMotorControllerSim.hpp"

#include "wpi/hal/SimDevice.h"
#include "wpi/units/length.hpp"

#include "wpi/simulation/SimDeviceSim.hpp"

using namespace frc;
using namespace frc::sim;

PWMMotorControllerSim::PWMMotorControllerSim(
    const PWMMotorController& motorctrl)
    : PWMMotorControllerSim(motorctrl.GetChannel()) {}

PWMMotorControllerSim::PWMMotorControllerSim(int channel) {
  frc::sim::SimDeviceSim deviceSim{"PWMMotorController", channel};
  m_simSpeed = deviceSim.GetDouble("Speed");
}

double PWMMotorControllerSim::GetSpeed() const {
  return m_simSpeed.Get();
}
