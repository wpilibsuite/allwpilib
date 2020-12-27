// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/DriverStation.h>

#include "subsystems/OnBoardIO.h"

OnBoardIO::OnBoardIO(OnBoardIO::ChannelMode dio1, OnBoardIO::ChannelMode dio2) {
  if(dio1 == ChannelMode::INPUT) {
    m_buttonB {1};
  } else {
    m_greenLed {1};
  }
  if (dio2 == ChannelMode::INPUT) {
    m_buttonC {2};
    m_redLed {2};
  }
}

bool OnBoardIO::GetButtonAPressed() { return m_buttonA.Get(); }

bool OnBoardIO::GetButtonBPressed() {
  if (m_buttonB != nullptr) {
    return m_buttonB.Get();
  }

  double currentTime = frc2::Timer::GetFPGATimestamp();
  if (currentTime > m_nextMessageTime) {
    DriverStation::ReportError("Button B was not configured");
    m_nextMessageTime = currentTime + MESSAGE_INTERVAL;
  }
  return false;
}

bool OnBoardIO::GetButtonCPressed() {
  if (m_buttonC != nullptr) {
    return m_buttonC.Get();
  }

  double currentTime = frc2::Timer::GetFPGATimestamp();
  if (currentTime > m_nextMessageTime) {
    DriverStation::ReportError("Button C was not configured");
    m_nextMessageTime = currentTime + MESSAGE_INTERVAL;
  }
  return false;
}

void OnBoardIO::SetGreenLed(bool value) {
  if (m_greenLed != nullptr) {
    m_greenLed.Set(value);
  } else {
    double currentTime = frc2::Timer::GetFPGATimestamp();
    if (currentTime > m_nextMessageTime) {
      DriverStation::ReportError("Green LED was not configured");
      m_nextMessageTime = currentTime + MESSAGE_INTERVAL;
    }
  }
}

void OnBoardIO::SetRedLed(bool value) {
  if (m_redLed != nullptr) {
    m_redLed.Set(value);
  } else {
    double currentTime = frc2::Timer::GetFPGATimestamp();
    if (currentTime > m_nextMessageTime) {
      DriverStation::ReportError("Red LED was not configured");
      m_nextMessageTime = currentTime + MESSAGE_INTERVAL;
    }
  }
}

void OnBoardIO::SetYellowLed(bool value) { m_yellowLed.Set(value); }
