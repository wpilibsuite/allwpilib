/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Buttons/JoystickButton.h>
#include <Joystick.h>

#include "Triggers/DoubleButton.h"

class OI {
public:
	OI();
	frc::Joystick* GetJoystick();

private:
	frc::Joystick joystick{0};

	frc::JoystickButton L1{&joystick, 11};
	frc::JoystickButton L2{&joystick, 9};
	frc::JoystickButton R1{&joystick, 12};
	frc::JoystickButton R2{&joystick, 10};

	DoubleButton sticks{&joystick, 2, 3};
};
