/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Pivot.h"

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
	AddChild("Upper Limit Switch", m_upperLimitSwitch);
	AddChild("Lower Limit Switch", m_lowerLimitSwitch);
	AddChild("Pot", m_pot);
	AddChild("Motor", m_motor);
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
