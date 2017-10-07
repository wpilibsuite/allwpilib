/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DoubleButton.h"

#include <Joystick.h>

DoubleButton::DoubleButton(frc::Joystick* joy, int button1, int button2) {
	this->joy = joy;
	this->button1 = button1;
	this->button2 = button2;
}

bool DoubleButton::Get() {
	return joy->GetRawButton(button1) && joy->GetRawButton(button2);
}
