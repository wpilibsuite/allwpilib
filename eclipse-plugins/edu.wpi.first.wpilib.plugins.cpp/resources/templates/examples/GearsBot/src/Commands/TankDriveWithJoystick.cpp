#include "TankDriveWithJoystick.h"
#include "Robot.h"

TankDriveWithJoystick::TankDriveWithJoystick() : Command("TankDriveWithJoystick") {
	Requires(Robot::drivetrain.get());
}

// Called just before this Command runs the first time
void TankDriveWithJoystick::Initialize() {}

// Called repeatedly when this Command is scheduled to run
void TankDriveWithJoystick::Execute() {
	Robot::drivetrain->Drive(Robot::oi->GetJoystick());
}

// Make this return true when this Command no longer needs to run execute()
bool TankDriveWithJoystick::IsFinished() {
	return false;
}

// Called once after isFinished returns true
void TankDriveWithJoystick::End() {
	Robot::drivetrain->Drive(0, 0);
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void TankDriveWithJoystick::Interrupted() {
	End();
}
