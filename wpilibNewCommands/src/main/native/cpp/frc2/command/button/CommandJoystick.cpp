// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandJoystick.h"

using namespace frc2;

CommandJoystick::CommandJoystick(int port)
    : CommandGenericHID(port), m_hid{frc::Joystick(port)} {}

frc::Joystick& CommandJoystick::GetHID() {
  return m_hid;
}

Trigger CommandJoystick::Trigger(frc::EventLoop* loop) const {
  return m_hid.Trigger(loop).CastTo<class Trigger>();
}

Trigger CommandJoystick::Top(frc::EventLoop* loop) const {
  return m_hid.Top(loop).CastTo<class Trigger>();
}

double CommandJoystick::GetMagnitude() const {
  return m_hid.GetMagnitude();
}

units::radian_t CommandJoystick::GetDirection() const {
  return m_hid.GetDirection();
}
