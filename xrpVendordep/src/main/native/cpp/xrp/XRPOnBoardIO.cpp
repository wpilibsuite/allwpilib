// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/xrp/XRPOnBoardIO.hpp"

#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/hardware/discrete/DigitalOutput.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/Timer.hpp"

using namespace frc;

bool XRPOnBoardIO::GetUserButtonPressed() {
  return m_userButton.Get();
}

void XRPOnBoardIO::SetLed(bool value) {
  m_led.Set(value);
}

bool XRPOnBoardIO::GetLed() const {
  return m_led.Get();
}
