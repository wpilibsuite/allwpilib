/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

double Wrist::GetMeasurement() { return m_pot.Get(); }

void Wrist::UseOutput(double output, double setpoint) { m_motor.Set(output); }

void Wrist::Periodic() { Log(); }
