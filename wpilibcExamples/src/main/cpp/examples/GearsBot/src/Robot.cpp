#include "Robot.h"

#include <iostream>

std::shared_ptr<DriveTrain> Robot::drivetrain = std::make_shared<DriveTrain>();
std::shared_ptr<Elevator> Robot::elevator = std::make_shared<Elevator>();
std::shared_ptr<Wrist> Robot::wrist = std::make_shared<Wrist>();
std::shared_ptr<Claw> Robot::claw = std::make_shared<Claw>();
std::unique_ptr<OI> Robot::oi = std::make_unique<OI>();

void Robot::RobotInit() {
	// Show what command your subsystem is running on the SmartDashboard
	frc::SmartDashboard::PutData(drivetrain.get());
	frc::SmartDashboard::PutData(elevator.get());
	frc::SmartDashboard::PutData(wrist.get());
	frc::SmartDashboard::PutData(claw.get());
}

void Robot::AutonomousInit() {
	autonomousCommand.Start();
	std::cout << "Starting Auto" << std::endl;
}

void Robot::AutonomousPeriodic() {
	frc::Scheduler::GetInstance()->Run();
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
	frc::Scheduler::GetInstance()->Run();
}

void Robot::TestPeriodic() {
	lw->Run();
}

START_ROBOT_CLASS(Robot)
