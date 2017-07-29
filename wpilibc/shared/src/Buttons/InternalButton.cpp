/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Buttons/InternalButton.h"

frc::InternalButton::InternalButton(bool inverted)
    : m_pressed(inverted), m_inverted(inverted) {}

void frc::InternalButton::SetInverted(bool inverted) { m_inverted = inverted; }

void frc::InternalButton::SetPressed(bool pressed) { m_pressed = pressed; }

bool frc::InternalButton::Get() { return m_pressed ^ m_inverted; }
