#include "Subsystems/Claw.h"

Claw::Claw() : Subsystem("Claw") {
    motor = new Victor(7);
    contact = new DigitalInput(5);

	// Let's show everything on the LiveWindow
    // TODO: LiveWindow::GetInstance()->AddActuator("Claw", "Motor", (Victor) motor);
    // TODO: contact
}

void Claw::Open()
{
	motor->Set(-1);
}


void Claw::Close()
{
	motor->Set(1);
}


void Claw::Stop() {
	motor->Set(0);
}

bool Claw::IsGripping() {
	return contact->Get();
}

