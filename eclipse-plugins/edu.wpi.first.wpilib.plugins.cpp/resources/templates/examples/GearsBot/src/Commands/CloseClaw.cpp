#include "CloseClaw.h"
#include "Robot.h"

CloseClaw::CloseClaw() : Command("CloseClaw") {
	Requires(Robot::claw.get());
}

// Called just before this Command runs the first time
void CloseClaw::Initialize() {
	Robot::claw->Close();
}

// Called repeatedly when this Command is scheduled to run
void CloseClaw::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool CloseClaw::IsFinished() {
	return Robot::claw->IsGripping() ;
}

// Called once after isFinished returns true
void CloseClaw::End() {
	// NOTE: Doesn't stop in simulation due to lower friction causing the can to fall out
	// + there is no need to worry about stalling the motor or crushing the can.
	#ifdef REAL
		Robot::claw->Stop();
	#endif
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void CloseClaw::Interrupted() {
	End();
}
