// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ServoSim.h"

#include <hal/SimDevice.h>
#include <units/length.h>

#include "frc/simulation/SimDeviceSim.h"

using namespace frc;
using namespace frc::sim;

ServoSim::ServoSim(const Servo& servo) : ServoSim(servo.GetChannel()) {}

ServoSim::ServoSim(int channel) {
  frc::sim::SimDeviceSim deviceSim{"Servo", channel};
  m_simPosition = deviceSim.GetDouble("Position");
}

double ServoSim::GetPosition() const {
  return m_simPosition.Get();
}

double ServoSim::GetAngle() const {
  return GetPosition() * Servo::GetServoAngleRange() + Servo::MIN_SERVO_ANGLE;
}
