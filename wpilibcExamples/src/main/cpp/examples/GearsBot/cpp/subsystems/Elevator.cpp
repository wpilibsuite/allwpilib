// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Elevator.h"

#include <frc/controller/PIDController.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/smartdashboard/SmartDashboard.h>

Elevator::Elevator()
    : frc2::PIDSubsystem(frc2::PIDController(kP_real, kI_real, 0)) {
#ifdef SIMULATION  // Check for simulation and update PID values
  GetPIDController()->SetPID(kP_simulation, kI_simulation, 0, 0);
#endif
  m_controller.SetTolerance(0.005);

  SetName("Elevator");
  // Let's show everything on the LiveWindow
  AddChild("Motor", &m_motor);
  AddChild("Pot", &m_pot);
}

void Elevator::Log() {
  frc::SmartDashboard::PutData("Wrist Pot", &m_pot);
}

double Elevator::GetMeasurement() {
  return m_pot.Get();
}

void Elevator::UseOutput(double output, double setpoint) {
  m_motor.Set(output);
}

void Elevator::Periodic() {
  Log();
}
