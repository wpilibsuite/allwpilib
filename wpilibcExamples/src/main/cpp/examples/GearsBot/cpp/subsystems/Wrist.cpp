// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Wrist.h"

#include <frc/controller/PIDController.h>
#include <frc/smartdashboard/SmartDashboard.h>

Wrist::Wrist() : frc2::PIDSubsystem(frc2::PIDController(kP_real, 0, 0)) {
#ifdef SIMULATION  // Check for simulation and update PID values
  GetPIDController()->SetPID(kP_simulation, 0, 0, 0);
#endif
  m_controller.SetTolerance(2.5);

  SetName("Wrist");
  // Let's show everything on the LiveWindow
  AddChild("Motor", &m_motor);
  AddChild("Pot", &m_pot);
}

void Wrist::Log() {
  frc::SmartDashboard::PutNumber("Wrist Angle", GetMeasurement());
}

double Wrist::GetMeasurement() {
  return m_pot.Get();
}

void Wrist::UseOutput(double output, double setpoint) {
  m_motor.Set(output);
}

void Wrist::Periodic() {
  Log();
}
