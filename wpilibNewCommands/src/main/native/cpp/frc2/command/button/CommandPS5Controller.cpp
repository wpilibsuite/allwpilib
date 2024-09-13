// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandPS5Controller.h"

using namespace frc2;

Trigger CommandPS5Controller::Button(int button, frc::EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Square(frc::EventLoop* loop) const {
  return PS5Controller::Square(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Cross(frc::EventLoop* loop) const {
  return PS5Controller::Cross(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Circle(frc::EventLoop* loop) const {
  return PS5Controller::Circle(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Triangle(frc::EventLoop* loop) const {
  return PS5Controller::Triangle(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::L1(frc::EventLoop* loop) const {
  return PS5Controller::L1(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::R1(frc::EventLoop* loop) const {
  return PS5Controller::R1(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::L2(frc::EventLoop* loop) const {
  return PS5Controller::L2(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::R2(frc::EventLoop* loop) const {
  return PS5Controller::R2(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Options(frc::EventLoop* loop) const {
  return PS5Controller::Options(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::L3(frc::EventLoop* loop) const {
  return PS5Controller::L3(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::R3(frc::EventLoop* loop) const {
  return PS5Controller::R3(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::PS(frc::EventLoop* loop) const {
  return PS5Controller::PS(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Touchpad(frc::EventLoop* loop) const {
  return PS5Controller::Touchpad(loop).CastTo<Trigger>();
}
