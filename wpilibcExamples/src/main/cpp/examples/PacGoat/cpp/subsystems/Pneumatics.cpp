// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Pneumatics.h"

#include <frc/smartdashboard/SmartDashboard.h>

Pneumatics::Pneumatics() : frc::Subsystem("Pneumatics") {
  AddChild("Pressure Sensor", m_pressureSensor);
}

void Pneumatics::InitDefaultCommand() {
  // No default command
}

void Pneumatics::Start() {
#ifndef SIMULATION
  m_compressor.Start();
#endif
}

bool Pneumatics::IsPressurized() {
#ifndef SIMULATION
  return kMaxPressure <= m_pressureSensor.GetVoltage();
#else
  return true;  // NOTE: Simulation always has full pressure
#endif
}

void Pneumatics::WritePressure() {
  frc::SmartDashboard::PutNumber("Pressure", m_pressureSensor.GetVoltage());
}
