/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "triggers/DoubleButton.h"

#include <frc/Joystick.h>

DoubleButton::DoubleButton(frc::Joystick* joy, int button1, int button2)
    : m_joy(*joy) {
  m_button1 = button1;
  m_button2 = button2;
}

bool DoubleButton::Get() {
  return m_joy.GetRawButton(m_button1) && m_joy.GetRawButton(m_button2);
}

bool DoubleButton::GetPressed() {
  m_button1Pressed |= m_joy.GetRawButtonPressed(m_button1);
  m_button2Pressed |= m_joy.GetRawButtonPressed(m_button2);
  if (m_button1Pressed && m_button2Pressed) {
    m_button1Pressed = false;
    m_button2Pressed = false;
    return true;
  } else {
    return false;
  }
}

bool DoubleButton::GetReleased() {
  m_button1Released |= m_joy.GetRawButtonReleased(m_button1);
  m_button2Released |= m_joy.GetRawButtonReleased(m_button2);
  if (m_button1Released && m_button2Released) {
    m_button1Released = false;
    m_button2Released = false;
    return true;
  } else {
    return false;
  }
}
