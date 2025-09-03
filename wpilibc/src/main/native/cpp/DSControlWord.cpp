// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DSControlWord.h"

#include <hal/DriverStation.h>
#include <hal/DriverStationTypes.h>

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

DSControlWord::RobotMode DSControlWord::GetRobotMode() const {
  return static_cast<DSControlWord::RobotMode>(m_controlWord.robotMode);
}

bool DSControlWord::IsAutonomous() const {
  return m_controlWord.robotMode == HAL_ROBOTMODE_AUTONOMOUS;
}

bool DSControlWord::IsAutonomousEnabled() const {
  return m_controlWord.robotMode == HAL_ROBOTMODE_AUTONOMOUS &&
         m_controlWord.enabled && m_controlWord.dsAttached;
}

bool DSControlWord::IsTeleop() const {
  return m_controlWord.robotMode == HAL_ROBOTMODE_TELEOPERATED;
}

bool DSControlWord::IsTeleopEnabled() const {
  return m_controlWord.robotMode == HAL_ROBOTMODE_TELEOPERATED &&
         m_controlWord.enabled && m_controlWord.dsAttached;
}

bool DSControlWord::IsTest() const {
  return m_controlWord.robotMode == HAL_ROBOTMODE_TEST;
}

bool DSControlWord::IsTestEnabled() const {
  return m_controlWord.robotMode == HAL_ROBOTMODE_TEST &&
         m_controlWord.enabled && m_controlWord.dsAttached;
}

bool DSControlWord::IsDSAttached() const {
  return m_controlWord.dsAttached;
}

bool DSControlWord::IsFMSAttached() const {
  return m_controlWord.fmsAttached;
}
