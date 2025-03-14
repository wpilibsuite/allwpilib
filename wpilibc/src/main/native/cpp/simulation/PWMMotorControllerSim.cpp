// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PWMMotorControllerSim.h"

#include <hal/SimDevice.h>
#include <units/length.h>

#include "frc/simulation/SimDeviceSim.h"

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
