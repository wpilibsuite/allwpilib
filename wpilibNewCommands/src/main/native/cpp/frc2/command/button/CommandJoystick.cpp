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
  return Button(frc::Joystick::ButtonType::kTriggerButton, loop);
}

Trigger CommandJoystick::Top(frc::EventLoop* loop) const {
  return Button(frc::Joystick::ButtonType::kTopButton, loop);
}

double CommandJoystick::GetMagnitude() const {
  return m_hid.GetMagnitude();
}

units::radian_t CommandJoystick::GetDirection() const {
  // https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html#joystick-and-controller-coordinate-system
  // +X is right and +Y is back, so 0 radians is right and CW is positive.
  // Rotate by 90 degrees CCW to make 0 radians forward and CW positive.
  return m_hid.GetDirection();
}
