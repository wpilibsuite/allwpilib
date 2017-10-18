/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DriveTrain.h"

#include <Joystick.h>
#include <LiveWindow/LiveWindow.h>

#include "../Commands/TankDriveWithJoystick.h"

DriveTrain::DriveTrain()
    : frc::Subsystem("DriveTrain") {
// Encoders may measure differently in the real world and in
// simulation. In this example the robot moves 0.042 barleycorns
// per tick in the real world, but the simulated encoders
// simulate 360 tick encoders. This if statement allows for the
// real robot to handle this difference in devices.
#ifndef SIMULATION
	leftEncoder.SetDistancePerPulse(0.042);
	rightEncoder.SetDistancePerPulse(0.042);
#else
	// Circumference in ft = 4in/12(in/ft)*PI
	leftEncoder.SetDistancePerPulse(
			static_cast<double>(4.0 / 12.0 * M_PI) / 360.0);
	rightEncoder.SetDistancePerPulse(
			static_cast<double>(4.0 / 12.0 * M_PI) / 360.0);
#endif

	// Let's show everything on the LiveWindow
	// frc::LiveWindow::GetInstance()->AddActuator("Drive Train",
	// 		"Front_Left Motor", &frontLeft);
	// frc::LiveWindow::GetInstance()->AddActuator("Drive Train",
	// 		"Rear Left Motor", &rearLeft);
	// frc::LiveWindow::GetInstance()->AddActuator("Drive Train",
	// 		"Front Right Motor", &frontRight);
	// frc::LiveWindow::GetInstance()->AddActuator("Drive Train",
	// 		"Rear Right Motor", &rearRight);
	// frc::LiveWindow::GetInstance()->AddSensor("Drive Train", "Left
	// Encoder",
	// 		&leftEncoder);
	// frc::LiveWindow::GetInstance()->AddSensor("Drive Train", "Right
	// Encoder",
	// 		&rightEncoder);
	// frc::LiveWindow::GetInstance()->AddSensor("Drive Train",
	// "Rangefinder",
	// 		&rangefinder);
	// frc::LiveWindow::GetInstance()->AddSensor("Drive Train", "Gyro",
	// &gyro);
}

/**
 * When no other command is running let the operator drive around
 * using the PS3 joystick.
 */
void DriveTrain::InitDefaultCommand() {
	SetDefaultCommand(new TankDriveWithJoystick());
}

/**
 * The log method puts interesting information to the SmartDashboard.
 */
void DriveTrain::Log() {
	frc::SmartDashboard::PutNumber(
			"Left Distance", leftEncoder.GetDistance());
	frc::SmartDashboard::PutNumber(
			"Right Distance", rightEncoder.GetDistance());
	frc::SmartDashboard::PutNumber("Left Speed", leftEncoder.GetRate());
	frc::SmartDashboard::PutNumber("Right Speed", rightEncoder.GetRate());
	frc::SmartDashboard::PutNumber("Gyro", gyro.GetAngle());
}

void DriveTrain::Drive(double left, double right) {
	drive.TankDrive(left, right);
}

void DriveTrain::Drive(frc::Joystick* joy) {
	Drive(-joy->GetY(), -joy->GetRawAxis(4));
}

double DriveTrain::GetHeading() {
	return gyro.GetAngle();
}

void DriveTrain::Reset() {
	gyro.Reset();
	leftEncoder.Reset();
	rightEncoder.Reset();
}

double DriveTrain::GetDistance() {
	return (leftEncoder.GetDistance() + rightEncoder.GetDistance()) / 2;
}

double DriveTrain::GetDistanceToObstacle() {
	// Really meters in simulation since it's a rangefinder...
	return rangefinder.GetAverageVoltage();
}
