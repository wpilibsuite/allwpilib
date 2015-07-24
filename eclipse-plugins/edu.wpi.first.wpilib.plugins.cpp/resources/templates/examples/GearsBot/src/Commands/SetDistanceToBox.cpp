#include "SetDistanceToBox.h"
#include "Robot.h"

SetDistanceToBox::SetDistanceToBox(double distance) {
	Requires(Robot::drivetrain.get());
    pid = new PIDController(-2, 0, 0, new SetDistanceToBoxPIDSource(),
    		                          new SetDistanceToBoxPIDOutput());
    pid->SetAbsoluteTolerance(0.01);
    pid->SetSetpoint(distance);
}

// Called just before this Command runs the first time
void SetDistanceToBox::Initialize() {
	// Get everything in a safe starting state.
    Robot::drivetrain->Reset();
	pid->Reset();
    pid->Enable();
}

// Called repeatedly when this Command is scheduled to run
void SetDistanceToBox::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool SetDistanceToBox::IsFinished() {
	return pid->OnTarget();
}

// Called once after isFinished returns true
void SetDistanceToBox::End() {
	// Stop PID and the wheels
	pid->Disable();
	Robot::drivetrain->Drive(0, 0);
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void SetDistanceToBox::Interrupted() {
	End();
}


SetDistanceToBoxPIDSource::~SetDistanceToBoxPIDSource() {}
double SetDistanceToBoxPIDSource::PIDGet() {
    return Robot::drivetrain->GetDistanceToObstacle();
}

SetDistanceToBoxPIDOutput::~SetDistanceToBoxPIDOutput() {}
void SetDistanceToBoxPIDOutput::PIDWrite(float d) {
    Robot::drivetrain->Drive(d, d);
}
