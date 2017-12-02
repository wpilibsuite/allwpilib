/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/buttons/InternalButton.h"

using namespace frc;

InternalButton::InternalButton(bool inverted)
    : m_state(inverted), m_lastState(inverted), m_inverted(inverted) {}

void InternalButton::SetInverted(bool inverted) { m_inverted = inverted; }

void InternalButton::SetPressed(bool pressed) {
  m_lastState = m_state;
  m_state = pressed;
  m_pressed |= !(m_lastState ^ m_inverted) && (m_state ^ m_inverted);
  m_released |= (m_lastState ^ m_inverted) && !(m_state ^ m_inverted);
}

bool InternalButton::Get() { return m_state ^ m_inverted; }

bool InternalButton::GetPressed() {
  m_pressed |= !(m_lastState ^ m_inverted) && (m_state ^ m_inverted);
  m_released |= (m_lastState ^ m_inverted) && !(m_state ^ m_inverted);
  m_lastState = m_state;

  if (m_pressed) {
    m_pressed = false;
    return true;
  } else {
    return false;
  }
}

bool InternalButton::GetReleased() {
  m_pressed |= !(m_lastState ^ m_inverted) && (m_state ^ m_inverted);
  m_released |= (m_lastState ^ m_inverted) && !(m_state ^ m_inverted);
  m_lastState = m_state;

  if (m_released) {
    m_released = false;
    return true;
  } else {
    return false;
  }
}
