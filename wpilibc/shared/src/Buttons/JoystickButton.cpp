/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/JoystickButton.h"

JoystickButton::JoystickButton(GenericHID *joystick, int buttonNumber)
    : m_joystick(joystick), m_buttonNumber(buttonNumber) {}

bool JoystickButton::Get() { return m_joystick->GetRawButton(m_buttonNumber); }
