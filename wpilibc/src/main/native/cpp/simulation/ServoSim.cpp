// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/ServoSim.hpp"

#include <wpi/hal/SimDevice.hpp>
#include <wpi/units/length.hpp>

#include "wpi/simulation/SimDeviceSim.hpp"

using namespace wpi;
using namespace wpi::sim;

ServoSim::ServoSim(const Servo& servo) : ServoSim(servo.GetChannel()) {}

ServoSim::ServoSim(int channel) {
  wpi::sim::SimDeviceSim deviceSim{"Servo", channel};
  m_simPosition = deviceSim.GetDouble("Position");
}

double ServoSim::GetPosition() const {
  return m_simPosition.Get();
}

double ServoSim::GetAngle() const {
  return GetPosition() * Servo::GetServoAngleRange() + Servo::kMinServoAngle;
}
