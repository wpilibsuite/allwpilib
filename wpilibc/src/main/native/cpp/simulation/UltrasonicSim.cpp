// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/UltrasonicSim.h"

#include "frc/Ultrasonic.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

UltrasonicSim::UltrasonicSim(const frc::Ultrasonic& ultrasonic)
    : UltrasonicSim(0, ultrasonic.GetEchoChannel()) {}

UltrasonicSim::UltrasonicSim(int ping, int echo) {
  frc::sim::SimDeviceSim deviceSim{"Ultrasonic", echo};
  m_simRangeValid = deviceSim.GetBoolean("Range Valid");
  m_simRange = deviceSim.GetDouble("Range (in)");
}

void UltrasonicSim::SetRangeValid(bool valid) {
  m_simRangeValid.Set(valid);
}

void UltrasonicSim::SetRange(units::inch_t range) {
  m_simRange.Set(range.value());
}
