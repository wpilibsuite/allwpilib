#include "Claw.h"

#include <LiveWindow/LiveWindow.h>

Claw::Claw() :
		frc::Subsystem("Claw") {
	// Let's show everything on the LiveWindow
	// frc::LiveWindow::GetInstance()->AddActuator("Claw", "Motor", &motor);
}

void Claw::InitDefaultCommand() {

}

void Claw::Open() {
	motor.Set(-1);
}

void Claw::Close() {
	motor.Set(1);
}

void Claw::Stop() {
	motor.Set(0);
}

bool Claw::IsGripping() {
	return contact.Get();
}

void Claw::Log() {

}
