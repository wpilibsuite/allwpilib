// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command2/button/CommandJoystick.h"

using namespace frc;

Trigger CommandJoystick::Button(int button, EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<class Trigger>();
}

Trigger CommandJoystick::Trigger(EventLoop* loop) const {
  return Joystick::Trigger(loop).CastTo<class Trigger>();
}

Trigger CommandJoystick::Top(EventLoop* loop) const {
  return Joystick::Top(loop).CastTo<class Trigger>();
}
