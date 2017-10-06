#include "DriveWithJoystick.h"

#include "../Robot.h"

DriveWithJoystick::DriveWithJoystick() {
	Requires(Robot::drivetrain.get());
}

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
