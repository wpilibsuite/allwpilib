/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/buttons/POVButton.h"

using namespace frc;

POVButton::POVButton(GenericHID& joystick, int angle, int povNumber)
    : m_joystick(&joystick), m_angle(angle), m_povNumber(povNumber) {}

bool POVButton::Get() { return m_joystick->GetPOV(m_povNumber) == m_angle; }
