/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/buttons/InternalButton.h"

using namespace frc;

InternalButton::InternalButton(bool inverted)
    : m_pressed(inverted), m_inverted(inverted) {}

void InternalButton::SetInverted(bool inverted) { m_inverted = inverted; }

void InternalButton::SetPressed(bool pressed) { m_pressed = pressed; }

bool InternalButton::Get() { return m_pressed ^ m_inverted; }
