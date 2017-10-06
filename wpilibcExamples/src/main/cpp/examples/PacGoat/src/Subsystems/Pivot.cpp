#include "Pivot.h"

#include <LiveWindow/LiveWindow.h>

Pivot::Pivot() :
		frc::PIDSubsystem("Pivot", 7.0, 0.0, 8.0) {
	SetAbsoluteTolerance(0.005);
	GetPIDController()->SetContinuous(false);
#ifdef SIMULATION
	// PID is different in simulation.
	GetPIDController()->SetPID(0.5, 0.001, 2);
	SetAbsoluteTolerance(5);
#endif

	// Put everything to the LiveWindow for testing.
	frc::LiveWindow::GetInstance()->AddSensor("Pivot", "Upper Limit Switch",
			&upperLimitSwitch);
	frc::LiveWindow::GetInstance()->AddSensor("Pivot", "Lower Limit Switch",
			&lowerLimitSwitch);
	// XXX: frc::LiveWindow::GetInstance()->AddSensor("Pivot", "Pot", &pot);
	// XXX: frc::LiveWindow::GetInstance()->AddActuator("Pivot", "Motor", &motor);
	frc::LiveWindow::GetInstance()->AddActuator("Pivot", "PIDSubsystem Controller",
			GetPIDController());
}

void InitDefaultCommand() {

}

double Pivot::ReturnPIDInput() {
	return pot.Get();
}

void Pivot::UsePIDOutput(double output) {
	motor.PIDWrite(output);
}

bool Pivot::IsAtUpperLimit() {
	return upperLimitSwitch.Get(); // TODO: inverted from real robot (prefix with !)
}

bool Pivot::IsAtLowerLimit() {
	return lowerLimitSwitch.Get(); // TODO: inverted from real robot (prefix with !)
}

double Pivot::GetAngle() {
	return pot.Get();
}
