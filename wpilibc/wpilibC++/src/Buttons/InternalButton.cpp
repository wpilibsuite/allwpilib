/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Buttons/InternalButton.h"

InternalButton::InternalButton(bool inverted)
    : m_pressed(inverted), m_inverted(inverted) {}

void InternalButton::SetInverted(bool inverted) { m_inverted = inverted; }

void InternalButton::SetPressed(bool pressed) { m_pressed = pressed; }

bool InternalButton::Get() { return m_pressed ^ m_inverted; }
