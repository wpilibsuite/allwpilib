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
	// Motor", &rollerMotor);
	LiveWindow::GetInstance()->AddSensor(
			"Collector", "Ball Detector", &ballDetector);
	LiveWindow::GetInstance()->AddSensor(
			"Collector", "Claw Open Detector", &openDetector);
	LiveWindow::GetInstance()->AddActuator("Collector", "Piston", &piston);
}

bool Collector::HasBall() {
	return ballDetector.Get();  // TODO: prepend ! to reflect real robot
}

void Collector::SetSpeed(double speed) {
	rollerMotor.Set(-speed);
}

void Collector::Stop() {
	rollerMotor.Set(0);
}

bool Collector::IsOpen() {
	return openDetector.Get();  // TODO: prepend ! to reflect real robot
}

void Collector::Open() {
	piston.Set(true);
}

void Collector::Close() {
	piston.Set(false);
}

void Collector::InitDefaultCommand() {}
