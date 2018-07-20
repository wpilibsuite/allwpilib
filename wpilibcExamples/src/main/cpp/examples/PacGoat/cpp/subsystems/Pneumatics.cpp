/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
