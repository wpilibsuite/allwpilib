// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/buttons/POVButton.h"

using namespace frc;

POVButton::POVButton(GenericHID& joystick, int angle, int povNumber)
    : m_joystick(&joystick), m_angle(angle), m_povNumber(povNumber) {}

bool POVButton::Get() {
  return m_joystick->GetPOV(m_povNumber) == m_angle;
}
