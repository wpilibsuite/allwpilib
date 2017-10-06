#include "CloseClaw.h"

#include "../Robot.h"

CloseClaw::CloseClaw() {
	Requires(Robot::collector.get());
}

// Called just before this Command runs the first time
void CloseClaw::Initialize() {
	Robot::collector->Close();
}
