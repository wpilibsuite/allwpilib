/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

void Elevator::Log() { frc::SmartDashboard::PutData("Wrist Pot", &m_pot); }

double Elevator::GetMeasurement() { return m_pot.Get(); }

void Elevator::UseOutput(double output, double setpoint) {
  m_motor.Set(output);
}

void Elevator::Periodic() { Log(); }
