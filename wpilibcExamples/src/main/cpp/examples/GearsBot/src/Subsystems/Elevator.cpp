/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Elevator.h"

#include <LiveWindow/LiveWindow.h>
#include <SmartDashboard/SmartDashboard.h>

Elevator::Elevator()
    : frc::PIDSubsystem("Elevator", kP_real, kI_real, 0.0) {
#ifdef SIMULATION  // Check for simulation and update PID values
	GetPIDController()->SetPID(kP_simulation, kI_simulation, 0, 0);
#endif
	SetAbsoluteTolerance(0.005);

	// Let's show everything on the LiveWindow
	// frc::LiveWindow::GetInstance()->AddActuator("Elevator", "Motor",
	// &motor);
	// frc::LiveWindow::GetInstance()->AddSensor("Elevator", "Pot", &pot);
	// frc::LiveWindow::GetInstance()->AddActuator("Elevator", "PID",
	// 		GetPIDController());
}

void Elevator::InitDefaultCommand() {}

void Elevator::Log() {
	// frc::SmartDashboard::PutData("Wrist Pot", &pot);
}

double Elevator::ReturnPIDInput() {
	return pot.Get();
}

void Elevator::UsePIDOutput(double d) {
	motor.Set(d);
}
