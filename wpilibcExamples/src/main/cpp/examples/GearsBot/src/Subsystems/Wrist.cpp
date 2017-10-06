#include "Wrist.h"

#include <LiveWindow/LiveWindow.h>
#include <SmartDashboard/SmartDashboard.h>

Wrist::Wrist() :
		frc::PIDSubsystem("Wrist", kP_real, 0.0, 0.0) {
#ifdef SIMULATION  // Check for simulation and update PID values
	GetPIDController()->SetPID(kP_simulation, 0, 0, 0);
#endif
	SetAbsoluteTolerance(2.5);

	// Let's show everything on the LiveWindow
	// frc::LiveWindow::GetInstance()->AddActuator("Wrist", "Motor", &motor);
	// frc::LiveWindow::GetInstance()->AddSensor("Wrist", "Pot", &pot);
	frc::LiveWindow::GetInstance()->AddActuator("Wrist", "PID", GetPIDController());
}

void Wrist::InitDefaultCommand() {

}

void Wrist::Log() {
	// frc::SmartDashboard::PutData("Wrist Angle", &pot);
}

double Wrist::ReturnPIDInput() {
	return pot.Get();
}

void Wrist::UsePIDOutput(double d) {
	motor.Set(d);
}
