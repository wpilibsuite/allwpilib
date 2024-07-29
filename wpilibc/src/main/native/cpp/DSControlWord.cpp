// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DSControlWord.h"

#include <hal/DriverStation.h>

using namespace frc;

DSControlWord::DSControlWord() {
  HAL_GetControlWord(&m_controlWord);
}

bool DSControlWord::IsEnabled() const {
  return m_controlWord.enabled && m_controlWord.dsAttached;
}

bool DSControlWord::IsDisabled() const {
  return !(m_controlWord.enabled && m_controlWord.dsAttached);
}

bool DSControlWord::IsEStopped() const {
  return m_controlWord.eStop;
}

bool DSControlWord::IsAutonomous() const {
  return m_controlWord.autonomous;
}

bool DSControlWord::IsAutonomousEnabled() const {
  return m_controlWord.autonomous && m_controlWord.enabled &&
         m_controlWord.dsAttached;
}

bool DSControlWord::IsTeleop() const {
  return !(m_controlWord.autonomous || m_controlWord.test);
}

bool DSControlWord::IsTeleopEnabled() const {
  return !m_controlWord.autonomous && !m_controlWord.test &&
         m_controlWord.enabled && m_controlWord.dsAttached;
}

bool DSControlWord::IsTest() const {
  return m_controlWord.test;
}

bool DSControlWord::IsDSAttached() const {
  return m_controlWord.dsAttached;
}

bool DSControlWord::IsFMSAttached() const {
  return m_controlWord.fmsAttached;
}
