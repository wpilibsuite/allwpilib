// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandStadiaController.h"

using namespace frc2;

Trigger CommandStadiaController::Button(int button,
                                        frc::EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::LeftBumper(frc::EventLoop* loop) const {
  return StadiaController::LeftBumper(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::RightBumper(frc::EventLoop* loop) const {
  return StadiaController::RightBumper(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::LeftStick(frc::EventLoop* loop) const {
  return StadiaController::LeftStick(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::RightStick(frc::EventLoop* loop) const {
  return StadiaController::RightStick(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::A(frc::EventLoop* loop) const {
  return StadiaController::A(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::B(frc::EventLoop* loop) const {
  return StadiaController::B(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::X(frc::EventLoop* loop) const {
  return StadiaController::X(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Y(frc::EventLoop* loop) const {
  return StadiaController::Y(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Ellipses(frc::EventLoop* loop) const {
  return StadiaController::Ellipses(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Hamburger(frc::EventLoop* loop) const {
  return StadiaController::Hamburger(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Stadia(frc::EventLoop* loop) const {
  return StadiaController::Stadia(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Google(frc::EventLoop* loop) const {
  return StadiaController::Google(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Frame(frc::EventLoop* loop) const {
  return StadiaController::Frame(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::LeftTrigger(frc::EventLoop* loop) const {
  return StadiaController::LeftTrigger(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::RightTrigger(frc::EventLoop* loop) const {
  return StadiaController::RightTrigger(loop).CastTo<Trigger>();
}
