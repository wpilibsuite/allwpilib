/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DoubleButton.h"

#include <Joystick.h>

DoubleButton::DoubleButton(frc::Joystick* joy, int button1, int button2)
    : m_joy(*joy) {
	m_button1 = button1;
	m_button2 = button2;
}

bool DoubleButton::Get() {
	return m_joy.GetRawButton(m_button1) && m_joy.GetRawButton(m_button2);
}
