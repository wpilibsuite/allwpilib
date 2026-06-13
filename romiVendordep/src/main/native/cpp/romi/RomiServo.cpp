// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/romi/RomiServo.hpp"

#include <numbers>
#include <string>

#include "fmt/format.h"
#include "wpi/romi/OnBoardIO.hpp"
#include "wpi/units/angle.hpp"

using namespace wpi::romi;

RomiServo::RomiServo(int channel) {
  OnBoardIO::AllocatePWM(channel);

  std::string deviceName = fmt::format("PWM:{}", channel);
  m_simDevice = hal::SimDevice(deviceName.c_str());

  if (m_simDevice) {
    m_simDevice.CreateBoolean("init", hal::SimDevice::Direction::OUTPUT, true);
    m_simPosition = m_simDevice.CreateDouble(
        "position", hal::SimDevice::Direction::OUTPUT, 0.5);
  }
}

void RomiServo::SetAngle(wpi::units::radian_t angle) {
  angle = std::clamp<wpi::units::radian_t>(angle, 0_deg, 180_deg);
  double pos = angle.value() / std::numbers::pi;

  if (m_simPosition) {
    m_simPosition.Set(pos);
  }
}

wpi::units::radian_t RomiServo::GetAngle() const {
  if (m_simPosition) {
    return wpi::units::radian_t{m_simPosition.Get() * std::numbers::pi};
  }

  return 90_deg;
}
