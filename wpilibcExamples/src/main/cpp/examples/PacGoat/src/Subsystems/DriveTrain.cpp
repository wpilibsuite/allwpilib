/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DriveTrain.h"

#include <cmath>

#include <Joystick.h>
#include <LiveWindow/LiveWindow.h>

#include "../Commands/DriveWithJoystick.h"

DriveTrain::DriveTrain()
    : frc::Subsystem("DriveTrain") {
	// frc::LiveWindow::GetInstance()->AddActuator("DriveTrain", "Front Left
	// CIM", &m_frontLeftCIM);
	// frc::LiveWindow::GetInstance()->AddActuator("DriveTrain", "Front
	// Right CIM", &m_frontRightCIM);
	// frc::LiveWindow::GetInstance()->AddActuator("DriveTrain", "Back Left
	// CIM", &m_backLeftCIM);
	// frc::LiveWindow::GetInstance()->AddActuator("DriveTrain", "Back Right
	// CIM", &m_backRightCIM);

	// Configure the DifferentialDrive to reflect the fact that all our
	// motors are wired backwards and our drivers sensitivity preferences.
	m_robotDrive.SetSafetyEnabled(false);
	m_robotDrive.SetExpiration(0.1);
	m_robotDrive.SetMaxOutput(1.0);
	m_leftCIMs.SetInverted(true);
	m_rightCIMs.SetInverted(true);

	// Configure encoders
	m_rightEncoder.SetPIDSourceType(PIDSourceType::kDisplacement);
	m_leftEncoder.SetPIDSourceType(PIDSourceType::kDisplacement);

#ifndef SIMULATION
	// Converts to feet
	m_rightEncoder.SetDistancePerPulse(0.0785398);
	m_leftEncoder.SetDistancePerPulse(0.0785398);
#else
	// Convert to feet 4in diameter wheels with 360 tick simulated encoders
	m_rightEncoder.SetDistancePerPulse(
			(4.0 /*in*/ * M_PI) / (360.0 * 12.0 /*in/ft*/));
	m_leftEncoder.SetDistancePerPulse(
			(4.0 /*in*/ * M_PI) / (360.0 * 12.0 /*in/ft*/));
#endif

	LiveWindow::GetInstance()->AddSensor(
			"DriveTrain", "Right Encoder", m_rightEncoder);
	LiveWindow::GetInstance()->AddSensor(
			"DriveTrain", "Left Encoder", m_leftEncoder);

// Configure gyro
#ifndef SIMULATION
	m_gyro.SetSensitivity(0.007);  // TODO: Handle more gracefully?
#endif
	LiveWindow::GetInstance()->AddSensor("DriveTrain", "Gyro", &m_gyro);
}

void DriveTrain::InitDefaultCommand() {
	SetDefaultCommand(new DriveWithJoystick());
}

void DriveTrain::TankDrive(double leftAxis, double rightAxis) {
	m_robotDrive.TankDrive(leftAxis, rightAxis);
}

void DriveTrain::Stop() {
	m_robotDrive.TankDrive(0.0, 0.0);
}

Encoder& DriveTrain::GetLeftEncoder() {
	return m_leftEncoder;
}

Encoder& DriveTrain::GetRightEncoder() {
	return m_rightEncoder;
}

double DriveTrain::GetAngle() {
	return m_gyro.GetAngle();
}
