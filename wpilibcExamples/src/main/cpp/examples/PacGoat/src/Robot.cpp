/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Robot.h"

#include <iostream>

#include <SmartDashboard/SmartDashboard.h>

std::shared_ptr<DriveTrain> Robot::drivetrain = std::make_shared<DriveTrain>();
std::shared_ptr<Pivot> Robot::pivot = std::make_shared<Pivot>();
std::shared_ptr<Collector> Robot::collector = std::make_shared<Collector>();
std::shared_ptr<Shooter> Robot::shooter = std::make_shared<Shooter>();
std::shared_ptr<Pneumatics> Robot::pneumatics = std::make_shared<Pneumatics>();
std::unique_ptr<OI> Robot::oi = std::make_unique<OI>();

void Robot::RobotInit() {
	// Show what command your subsystem is running on the SmartDashboard
	frc::SmartDashboard::PutData(drivetrain.get());
	frc::SmartDashboard::PutData(pivot.get());
	frc::SmartDashboard::PutData(collector.get());
	frc::SmartDashboard::PutData(shooter.get());
	frc::SmartDashboard::PutData(pneumatics.get());

	// instantiate the command used for the autonomous period
	autoChooser.AddDefault("Drive and Shoot", driveAndShootAuto.get());
	autoChooser.AddObject("Drive Forward", driveForwardAuto.get());
	frc::SmartDashboard::PutData("Auto Mode", &autoChooser);

	pneumatics->Start();  // Pressurize the pneumatics.
}

void Robot::AutonomousInit() {
	autonomousCommand = autoChooser.GetSelected();
	autonomousCommand->Start();
}

void Robot::AutonomousPeriodic() {
	frc::Scheduler::GetInstance()->Run();
	Log();
}

void Robot::TeleopInit() {
	// This makes sure that the autonomous stops running when
	// teleop starts running. If you want the autonomous to
	// continue until interrupted by another command, remove
	// this line or comment it out.
	if (autonomousCommand != nullptr) {
		autonomousCommand->Cancel();
	}
	std::cout << "Starting Teleop" << std::endl;
}

void Robot::TeleopPeriodic() {
	frc::Scheduler::GetInstance()->Run();
	Log();
}

void Robot::TestPeriodic() {
	frc::LiveWindow::GetInstance()->Run();
}

void Robot::DisabledInit() {
	shooter->Unlatch();
}

void Robot::DisabledPeriodic() {
	Log();
}

/**
 * Log interesting values to the SmartDashboard.
 */
void Robot::Log() {
	Robot::pneumatics->WritePressure();
	frc::SmartDashboard::PutNumber("Pivot Pot Value", pivot->GetAngle());
	frc::SmartDashboard::PutNumber("Left Distance",
			drivetrain->GetLeftEncoder()->GetDistance());
	frc::SmartDashboard::PutNumber("Right Distance",
			drivetrain->GetRightEncoder()->GetDistance());
}

START_ROBOT_CLASS(Robot)
