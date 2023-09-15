// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/command2/button/CommandPS4Controller.h"

using namespace frc;

Trigger CommandPS4Controller::Button(int button, EventLoop* loop) const {
  return GenericHID::Button(button, loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Square(EventLoop* loop) const {
  return PS4Controller::Square(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Cross(EventLoop* loop) const {
  return PS4Controller::Cross(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Circle(EventLoop* loop) const {
  return PS4Controller::Circle(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Triangle(EventLoop* loop) const {
  return PS4Controller::Triangle(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L1(EventLoop* loop) const {
  return PS4Controller::L1(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R1(EventLoop* loop) const {
  return PS4Controller::R1(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L2(EventLoop* loop) const {
  return PS4Controller::L2(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R2(EventLoop* loop) const {
  return PS4Controller::R2(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Options(EventLoop* loop) const {
  return PS4Controller::Options(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L3(EventLoop* loop) const {
  return PS4Controller::L3(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R3(EventLoop* loop) const {
  return PS4Controller::R3(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::PS(EventLoop* loop) const {
  return PS4Controller::PS(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Touchpad(EventLoop* loop) const {
  return PS4Controller::Touchpad(loop).CastTo<Trigger>();
}
