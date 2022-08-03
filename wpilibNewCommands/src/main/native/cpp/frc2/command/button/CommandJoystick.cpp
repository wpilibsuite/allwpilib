// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandJoystick.h"

using namespace frc2;

Trigger CommandJoystick::Button(int button, frc::EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<class Trigger>();
}

Trigger CommandJoystick::Trigger(frc::EventLoop* loop) const {
  return Joystick::Trigger(loop).CastTo<class Trigger>();
}

Trigger CommandJoystick::Top(frc::EventLoop* loop) const {
  return Joystick::Top(loop).CastTo<class Trigger>();
}
