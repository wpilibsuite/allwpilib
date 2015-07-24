
#include "Robot.h"

std::shared_ptr<DriveTrain> Robot::drivetrain;
std::shared_ptr<Elevator> Robot::elevator;
std::shared_ptr<Wrist> Robot::wrist;
std::shared_ptr<Claw> Robot::claw;

std::unique_ptr<OI> Robot::oi;

void Robot::RobotInit() {
	drivetrain.reset(new DriveTrain());
	elevator.reset(new Elevator());
	wrist.reset(new Wrist());
	claw.reset(new Claw());

	oi.reset(new OI());

	// Show what command your subsystem is running on the SmartDashboard
	SmartDashboard::PutData(drivetrain.get());
	SmartDashboard::PutData(elevator.get());
	SmartDashboard::PutData(wrist.get());
	SmartDashboard::PutData(claw.get());
}

void Robot::AutonomousInit() {
	autonomousCommand.Start();
	std::cout << "Starting Auto" << std::endl;
}

void Robot::AutonomousPeriodic() {
	Scheduler::GetInstance()->Run();
}

void Robot::TeleopInit() {
	// This makes sure that the autonomous stops running when
	// teleop starts running. If you want the autonomous to
	// continue until interrupted by another command, remove
	// this line or comment it out.
	autonomousCommand.Cancel();
	std::cout << "Starting Teleop" << std::endl;
}

void Robot::TeleopPeriodic() {
	Scheduler::GetInstance()->Run();
}

void Robot::TestPeriodic() {
	lw->Run();
}

START_ROBOT_CLASS(Robot)
