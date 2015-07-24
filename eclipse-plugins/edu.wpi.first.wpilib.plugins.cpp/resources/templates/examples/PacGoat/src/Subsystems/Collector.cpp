#include "Collector.h"

Collector::Collector() :
		Subsystem("Collector"),
		// Configure devices
		rollerMotor(new Victor(6)),
		ballDetector(new DigitalInput(10)),
		piston(new Solenoid(1)),
		openDetector(new DigitalInput(6))
{

	// Put everything to the LiveWindow for testing.
	// XXX: LiveWindow::GetInstance()->AddActuator("Collector", "Roller Motor", (Victor) rollerMotor);
	LiveWindow::GetInstance().AddSensor("Collector", "Ball Detector", ballDetector);
	LiveWindow::GetInstance().AddSensor("Collector", "Claw Open Detector", openDetector);
	LiveWindow::GetInstance().AddActuator("Collector", "Piston", piston);
}

bool Collector::HasBall() {
	return ballDetector->Get(); // TODO: prepend ! to reflect real robot
}

void Collector::SetSpeed(double speed) {
	rollerMotor->Set(-speed);
}

void Collector::Stop() {
	rollerMotor->Set(0);
}

bool Collector::IsOpen() {
	return openDetector->Get(); // TODO: prepend ! to reflect real robot
}

void Collector::Open() {
	piston->Set(true);
}

void Collector::Close() {
	piston->Set(false);
}

void Collector::InitDefaultCommand() {}
