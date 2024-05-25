// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sim/StorageSim.h"

void StorageSim::SimulationPeriodic() {}

double StorageSim::GetMotor() const {
  return m_motor.GetSpeed();
}

void StorageSim::SetIsFull(bool full) {
  m_ballSensor.SetValue(full);
}
