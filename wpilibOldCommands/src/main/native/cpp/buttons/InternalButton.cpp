// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/buttons/InternalButton.h"

using namespace frc;

InternalButton::InternalButton(bool inverted)
    : m_pressed(inverted), m_inverted(inverted) {}

void InternalButton::SetInverted(bool inverted) {
  m_inverted = inverted;
}

void InternalButton::SetPressed(bool pressed) {
  m_pressed = pressed;
}

bool InternalButton::Get() {
  return m_pressed ^ m_inverted;
}
