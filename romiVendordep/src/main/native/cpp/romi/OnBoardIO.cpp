// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <frc/DigitalInput.h>
#include <frc/DigitalOutput.h>
#include <frc/Errors.h>
#include <frc/Timer.h>
#include <frc/romi/OnBoardIO.h>

using namespace frc;

OnBoardIO::OnBoardIO(OnBoardIO::ChannelMode dio1, OnBoardIO::ChannelMode dio2) {
  if (dio1 == ChannelMode::INPUT) {
    m_buttonB = std::make_unique<frc::DigitalInput>(1);
  } else {
    m_greenLed = std::make_unique<frc::DigitalOutput>(1);
  }
  if (dio2 == ChannelMode::INPUT) {
    m_buttonC = std::make_unique<frc::DigitalInput>(2);
  } else {
    m_redLed = std::make_unique<frc::DigitalOutput>(2);
  }
}

bool OnBoardIO::GetButtonAPressed() {
  return m_buttonA.Get();
}

bool OnBoardIO::GetButtonBPressed() {
  if (m_buttonB) {
    return m_buttonB->Get();
  }

  auto currentTime = frc::Timer::GetTimestamp();
  if (currentTime > m_nextMessageTime) {
    FRC_ReportError(frc::err::Error, "{}", "Button B was not configured");
    m_nextMessageTime = currentTime + kMessageInterval;
  }
  return false;
}

bool OnBoardIO::GetButtonCPressed() {
  if (m_buttonC) {
    return m_buttonC->Get();
  }

  auto currentTime = frc::Timer::GetTimestamp();
  if (currentTime > m_nextMessageTime) {
    FRC_ReportError(frc::err::Error, "{}", "Button C was not configured");
    m_nextMessageTime = currentTime + kMessageInterval;
  }
  return false;
}

void OnBoardIO::SetGreenLed(bool value) {
  if (m_greenLed) {
    m_greenLed->Set(value);
  } else {
    auto currentTime = frc::Timer::GetTimestamp();
    if (currentTime > m_nextMessageTime) {
      FRC_ReportError(frc::err::Error, "{}", "Green LED was not configured");
      m_nextMessageTime = currentTime + kMessageInterval;
    }
  }
}

void OnBoardIO::SetRedLed(bool value) {
  if (m_redLed) {
    m_redLed->Set(value);
  } else {
    auto currentTime = frc::Timer::GetTimestamp();
    if (currentTime > m_nextMessageTime) {
      FRC_ReportError(frc::err::Error, "{}", "Red LED was not configured");
      m_nextMessageTime = currentTime + kMessageInterval;
    }
  }
}

void OnBoardIO::SetYellowLed(bool value) {
  m_yellowLed.Set(value);
}
