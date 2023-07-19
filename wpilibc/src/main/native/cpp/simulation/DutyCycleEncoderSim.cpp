// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DutyCycleEncoderSim.h"

#include "frc/DutyCycleEncoder.h"
#include "frc/simulation/SimDeviceSim.h"

using namespace frc::sim;

DutyCycleEncoderSim::DutyCycleEncoderSim(const frc::DutyCycleEncoder& encoder)
    : DutyCycleEncoderSim{encoder.GetSourceChannel()} {}

DutyCycleEncoderSim::DutyCycleEncoderSim(int channel) {
  frc::sim::SimDeviceSim deviceSim{"DutyCycle:DutyCycleEncoder", channel};
  m_simPosition = deviceSim.GetDouble("position");
  m_simDistancePerRotation = deviceSim.GetDouble("distance_per_rot");
  m_simAbsolutePosition = deviceSim.GetDouble("absPosition");
  m_simIsConnected = deviceSim.GetBoolean("connected");
}

double DutyCycleEncoderSim::Get() {
  return m_simPosition.Get();
}

void DutyCycleEncoderSim::Set(units::turn_t turns) {
  m_simPosition.Set(turns.value());
}

double DutyCycleEncoderSim::GetDistance() {
  return m_simPosition.Get() * m_simDistancePerRotation.Get();
}

void DutyCycleEncoderSim::SetDistance(double distance) {
  m_simPosition.Set(distance / m_simDistancePerRotation.Get());
}

double DutyCycleEncoderSim::GetAbsolutePosition() {
  return m_simAbsolutePosition.Get();
}

void DutyCycleEncoderSim::SetAbsolutePosition(double position) {
  m_simAbsolutePosition.Set(position);
}

double DutyCycleEncoderSim::GetDistancePerRotation() {
  return m_simDistancePerRotation.Get();
}

bool DutyCycleEncoderSim::IsConnected() {
  return m_simIsConnected.Get();
}

void DutyCycleEncoderSim::SetConnected(bool isConnected) {
  m_simIsConnected.Set(isConnected);
}
