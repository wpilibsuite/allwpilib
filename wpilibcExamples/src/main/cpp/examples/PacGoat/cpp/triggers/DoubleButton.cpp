// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "triggers/DoubleButton.h"

#include <frc/GenericHID.h>

DoubleButton::DoubleButton(frc::GenericHID* joy, int button1, int button2)
    : m_joy(*joy) {
  m_button1 = button1;
  m_button2 = button2;
}

bool DoubleButton::Get() {
  return m_joy.GetRawButton(m_button1) && m_joy.GetRawButton(m_button2);
}
