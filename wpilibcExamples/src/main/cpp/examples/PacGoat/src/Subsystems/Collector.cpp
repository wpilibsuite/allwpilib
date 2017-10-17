/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Collector.h"

#include <LiveWindow/LiveWindow.h>

Collector::Collector()
    : frc::Subsystem("Collector") {
	// Put everything to the LiveWindow for testing.
	// XXX: LiveWindow::GetInstance()->AddActuator("Collector", "Roller
	// Motor", &m_rollerMotor);
	LiveWindow::GetInstance()->AddSensor(
			"Collector", "Ball Detector", &m_ballDetector);
	LiveWindow::GetInstance()->AddSensor(
			"Collector", "Claw Open Detector", &m_openDetector);
	LiveWindow::GetInstance()->AddActuator(
			"Collector", "Piston", &m_piston);
}

bool Collector::HasBall() {
	return m_ballDetector.Get();  // TODO: prepend ! to reflect real robot
}

void Collector::SetSpeed(double speed) {
	m_rollerMotor.Set(-speed);
}

void Collector::Stop() {
	m_rollerMotor.Set(0);
}

bool Collector::IsOpen() {
	return m_openDetector.Get();  // TODO: prepend ! to reflect real robot
}

void Collector::Open() {
	m_piston.Set(true);
}

void Collector::Close() {
	m_piston.Set(false);
}

void Collector::InitDefaultCommand() {}
