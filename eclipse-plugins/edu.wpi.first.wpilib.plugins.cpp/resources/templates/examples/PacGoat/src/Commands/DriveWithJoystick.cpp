#include "DriveWithJoystick.h"

#include "Robot.h"

DriveWithJoystick::DriveWithJoystick() {
	Requires(Robot::drivetrain.get());
}

// Called just before this Command runs the first time
void DriveWithJoystick::Initialize() {}

// Called repeatedly when this Command is scheduled to run
void DriveWithJoystick::Execute() {
	Robot::drivetrain->TankDrive(Robot::oi->GetJoystick());
}

// Make this return true when this Command no longer needs to run execute()
bool DriveWithJoystick::IsFinished() {
	return false;
}

// Called once after isFinished returns true
void DriveWithJoystick::End() {
	Robot::drivetrain->Stop();
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void DriveWithJoystick::Interrupted() {
	End();
}
