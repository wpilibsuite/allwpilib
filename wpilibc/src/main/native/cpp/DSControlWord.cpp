// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DSControlWord.h"

#include <hal/DriverStation.h>

using namespace frc;

DSControlWord::DSControlWord() {
  HAL_GetControlWord(&m_controlWord);
}

bool DSControlWord::IsEStopped() const {
  return m_controlWord.eStop;
}

bool DSControlWord::IsDSAttached() const {
  return m_controlWord.dsAttached;
}

bool DSControlWord::IsFMSAttached() const {
  return m_controlWord.fmsAttached;
}
