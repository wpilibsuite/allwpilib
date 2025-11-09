// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandJoystick.hpp"

using namespace wpi::cmd;

CommandJoystick::CommandJoystick(int port)
    : CommandGenericHID(port), m_hid{wpi::Joystick(port)} {}

wpi::Joystick& CommandJoystick::GetHID() {
  return m_hid;
}

Trigger CommandJoystick::Trigger(wpi::EventLoop* loop) const {
  return Button(wpi::Joystick::ButtonType::kTriggerButton, loop);
}

Trigger CommandJoystick::Top(wpi::EventLoop* loop) const {
  return Button(wpi::Joystick::ButtonType::kTopButton, loop);
}

double CommandJoystick::GetMagnitude() const {
  return m_hid.GetMagnitude();
}

wpi::units::radian_t CommandJoystick::GetDirection() const {
  // https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html#joystick-and-controller-coordinate-system
  // A positive rotation around the X axis moves the joystick right, and a
  // positive rotation around the Y axis moves the joystick backward. When
  // treating them as translations, 0 radians is measured from the right
  // direction, and angle increases clockwise.
  //
  // It's rotated 90 degrees CCW (y is negated and the arguments are reversed)
  // so that 0 radians is forward.
  return m_hid.GetDirection();
}
