// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command2/button/CommandXboxController.h"

using namespace frc;

Trigger CommandXboxController::Button(int button, EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<Trigger>();
}

Trigger CommandXboxController::LeftBumper(EventLoop* loop) const {
  return XboxController::LeftBumper(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::RightBumper(EventLoop* loop) const {
  return XboxController::RightBumper(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::LeftStick(EventLoop* loop) const {
  return XboxController::LeftStick(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::RightStick(EventLoop* loop) const {
  return XboxController::RightStick(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::A(EventLoop* loop) const {
  return XboxController::A(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::B(EventLoop* loop) const {
  return XboxController::B(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::X(EventLoop* loop) const {
  return XboxController::X(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Y(EventLoop* loop) const {
  return XboxController::Y(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Back(EventLoop* loop) const {
  return XboxController::Back(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Start(EventLoop* loop) const {
  return XboxController::Start(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::LeftTrigger(double threshold,
                                           EventLoop* loop) const {
  return XboxController::LeftTrigger(threshold, loop).CastTo<Trigger>();
}

Trigger CommandXboxController::RightTrigger(double threshold,
                                            EventLoop* loop) const {
  return XboxController::RightTrigger(threshold, loop).CastTo<Trigger>();
}
