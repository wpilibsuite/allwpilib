/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Buttons/JoystickButton.h>
#include <Joystick.h>

class OI {
public:
	OI();
	frc::Joystick& GetJoystick();

private:
	frc::Joystick m_joy{0};

	// Create some buttons
	frc::JoystickButton m_dUp{&m_joy, 5};
	frc::JoystickButton m_dRight{&m_joy, 6};
	frc::JoystickButton m_dDown{&m_joy, 7};
	frc::JoystickButton m_dLeft{&m_joy, 8};
	frc::JoystickButton m_l2{&m_joy, 9};
	frc::JoystickButton m_r2{&m_joy, 10};
	frc::JoystickButton m_l1{&m_joy, 11};
	frc::JoystickButton m_r1{&m_joy, 12};
};
