// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PWMMotorControllerSim.h"

#include <hal/SimDevice.h>
#include <units/length.h>

#include "frc/simulation/SimDeviceSim.h"

using namespace frc;

PWMMotorControllerSim::PWMMotorControllerSim(
    const PWMMotorController& motorctrl)
    : PWMMotorControllerSim(motorctrl.GetName().c_str(),
                            motorctrl.GetChannel()) {}

PWMMotorControllerSim::PWMMotorControllerSim(const char* name, int channel) {
  frc::sim::SimDeviceSim deviceSim{name, channel};
  m_simSpeed = deviceSim.GetDouble("Speed");
}

double PWMMotorControllerSim::GetSpeed() const {
  return m_simSpeed.Get();
}
