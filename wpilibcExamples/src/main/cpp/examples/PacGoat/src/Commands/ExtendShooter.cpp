#include "ExtendShooter.h"

#include "../Robot.h"

ExtendShooter::ExtendShooter() :
		frc::TimedCommand(1.0) {
	Requires(Robot::shooter.get());
}

// Called just before this Command runs the first time
void ExtendShooter::Initialize() {
	Robot::shooter->ExtendBoth();
}

// Called once after isFinished returns true
void ExtendShooter::End() {
	Robot::shooter->RetractBoth();
}
