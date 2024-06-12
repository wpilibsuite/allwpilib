// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandPS4Controller.h"

using namespace frc2;

Trigger CommandPS4Controller::Button(int button, frc::EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Square(frc::EventLoop* loop) const {
  return PS4Controller::Square(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Cross(frc::EventLoop* loop) const {
  return PS4Controller::Cross(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Circle(frc::EventLoop* loop) const {
  return PS4Controller::Circle(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Triangle(frc::EventLoop* loop) const {
  return PS4Controller::Triangle(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L1(frc::EventLoop* loop) const {
  return PS4Controller::L1(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R1(frc::EventLoop* loop) const {
  return PS4Controller::R1(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L2(frc::EventLoop* loop) const {
  return PS4Controller::L2(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R2(frc::EventLoop* loop) const {
  return PS4Controller::R2(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Options(frc::EventLoop* loop) const {
  return PS4Controller::Options(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L3(frc::EventLoop* loop) const {
  return PS4Controller::L3(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R3(frc::EventLoop* loop) const {
  return PS4Controller::R3(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::PS(frc::EventLoop* loop) const {
  return PS4Controller::PS(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Touchpad(frc::EventLoop* loop) const {
  return PS4Controller::Touchpad(loop).CastTo<Trigger>();
}
