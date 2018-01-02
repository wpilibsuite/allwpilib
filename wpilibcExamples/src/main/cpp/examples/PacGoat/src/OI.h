/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
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
	frc::Joystick& GetJoystick();

private:
	frc::Joystick m_joystick{0};

	frc::JoystickButton m_l1{&m_joystick, 11};
	frc::JoystickButton m_l2{&m_joystick, 9};
	frc::JoystickButton m_r1{&m_joystick, 12};
	frc::JoystickButton m_r2{&m_joystick, 10};

	DoubleButton m_sticks{&m_joystick, 2, 3};
};
