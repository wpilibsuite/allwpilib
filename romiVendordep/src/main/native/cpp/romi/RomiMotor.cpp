// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/romi/RomiMotor.hpp"

#include "fmt/format.h"
#include "wpi/romi/OnBoardIO.hpp"

using namespace wpi::romi;

RomiMotor::RomiMotor(int channel) {
  OnBoardIO::AllocatePWM(channel);

  m_deviceName = fmt::format("PWM:{}", channel);
  m_simDevice = hal::SimDevice(m_deviceName.c_str());

  if (m_simDevice) {
    m_simDevice.CreateBoolean("init", hal::SimDevice::Direction::OUTPUT, true);
    m_simSpeed = m_simDevice.CreateDouble(
        "speed", hal::SimDevice::Direction::OUTPUT, 0.0);
  }
}

void RomiMotor::SetThrottle(double throttle) {
  if (m_simSpeed) {
    m_simSpeed.Set(m_inverted ? -throttle : throttle);
  }
}

double RomiMotor::GetThrottle() const {
  if (m_simSpeed) {
    return m_simSpeed.Get();
  }

  return 0.0;
}

void RomiMotor::SetInverted(bool isInverted) {
  m_inverted = isInverted;
}

bool RomiMotor::GetInverted() const {
  return m_inverted;
}

void RomiMotor::Disable() {
  SetThrottle(0.0);
}

void RomiMotor::StopMotor() {
  SetThrottle(0.0);
}

std::string RomiMotor::GetDescription() const {
  return m_deviceName;
}
