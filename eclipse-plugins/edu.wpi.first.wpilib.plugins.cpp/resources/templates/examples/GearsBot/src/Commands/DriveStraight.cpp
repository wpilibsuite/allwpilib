#include "DriveStraight.h"
#include "Robot.h"

DriveStraight::DriveStraight(double distance) {
	Requires(Robot::drivetrain.get());
    pid = new PIDController(4, 0, 0, new DriveStraightPIDSource(),
    		                         new DriveStraightPIDOutput());
    pid->SetAbsoluteTolerance(0.01);
    pid->SetSetpoint(distance);
}

// Called just before this Command runs the first time
void DriveStraight::Initialize() {
	// Get everything in a safe starting state.
    Robot::drivetrain->Reset();
	pid->Reset();
    pid->Enable();
}

// Called repeatedly when this Command is scheduled to run
void DriveStraight::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool DriveStraight::IsFinished() {
	return pid->OnTarget();
}

// Called once after isFinished returns true
void DriveStraight::End() {
	// Stop PID and the wheels
	pid->Disable();
	Robot::drivetrain->Drive(0, 0);
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void DriveStraight::Interrupted() {
	End();
}


DriveStraightPIDSource::~DriveStraightPIDSource() {}
double DriveStraightPIDSource::PIDGet() {
    return Robot::drivetrain->GetDistance();
}

DriveStraightPIDOutput::~DriveStraightPIDOutput() {}
void DriveStraightPIDOutput::PIDWrite(float d) {
    Robot::drivetrain->Drive(d, d);
}
