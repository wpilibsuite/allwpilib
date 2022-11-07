// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandXboxController.h"

using namespace frc2;

Trigger CommandXboxController::Button(int button, frc::EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<Trigger>();
}

Trigger CommandXboxController::LeftBumper(frc::EventLoop* loop) const {
  return XboxController::LeftBumper(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::RightBumper(frc::EventLoop* loop) const {
  return XboxController::RightBumper(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::LeftStick(frc::EventLoop* loop) const {
  return XboxController::LeftStick(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::RightStick(frc::EventLoop* loop) const {
  return XboxController::RightStick(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::A(frc::EventLoop* loop) const {
  return XboxController::A(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::B(frc::EventLoop* loop) const {
  return XboxController::B(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::X(frc::EventLoop* loop) const {
  return XboxController::X(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Y(frc::EventLoop* loop) const {
  return XboxController::Y(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Back(frc::EventLoop* loop) const {
  return XboxController::Back(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Start(frc::EventLoop* loop) const {
  return XboxController::Start(loop).CastTo<Trigger>();
}
