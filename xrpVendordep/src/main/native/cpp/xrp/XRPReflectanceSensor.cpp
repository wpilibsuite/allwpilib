// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/xrp/XRPReflectanceSensor.hpp"

using namespace wpi::xrp;

double XRPReflectanceSensor::GetLeftReflectanceValue() const {
  return m_leftSensor.GetVoltage() / 5.0;
}

double XRPReflectanceSensor::GetRightReflectanceValue() const {
  return m_rightSensor.GetVoltage() / 5.0;
}
