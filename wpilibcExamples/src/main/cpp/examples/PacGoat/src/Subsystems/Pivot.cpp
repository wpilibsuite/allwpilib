/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Pivot.h"

#include <LiveWindow/LiveWindow.h>

Pivot::Pivot()
    : frc::PIDSubsystem("Pivot", 7.0, 0.0, 8.0) {
	SetAbsoluteTolerance(0.005);
	GetPIDController()->SetContinuous(false);
#ifdef SIMULATION
	// PID is different in simulation.
	GetPIDController()->SetPID(0.5, 0.001, 2);
	SetAbsoluteTolerance(5);
#endif

	// Put everything to the LiveWindow for testing.
	frc::LiveWindow::GetInstance()->AddSensor(
			"Pivot", "Upper Limit Switch", &m_upperLimitSwitch);
	frc::LiveWindow::GetInstance()->AddSensor(
			"Pivot", "Lower Limit Switch", &m_lowerLimitSwitch);
	// XXX: frc::LiveWindow::GetInstance()->AddSensor("Pivot", "Pot",
	// &m_pot);
	// XXX: frc::LiveWindow::GetInstance()->AddActuator("Pivot", "Motor",
	// &m_motor);
	frc::LiveWindow::GetInstance()->AddActuator(
			"Pivot", "PIDSubsystem Controller", GetPIDController());
}

void InitDefaultCommand() {}

double Pivot::ReturnPIDInput() {
	return m_pot.Get();
}

void Pivot::UsePIDOutput(double output) {
	m_motor.PIDWrite(output);
}

bool Pivot::IsAtUpperLimit() {
	return m_upperLimitSwitch.Get();  // TODO: inverted from real robot
					  // (prefix with !)
}

bool Pivot::IsAtLowerLimit() {
	return m_lowerLimitSwitch.Get();  // TODO: inverted from real robot
					  // (prefix with !)
}

double Pivot::GetAngle() {
	return m_pot.Get();
}
