// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandPS5Controller.h"

using namespace frc2;

CommandPS5Controller::CommandPS5Controller(int port)
    : CommandGenericHID(port), m_hid{frc::PS5Controller(port)} {}

frc::PS5Controller& CommandPS5Controller::GetHID() {
  return m_hid;
}

Trigger CommandPS5Controller::Square(frc::EventLoop* loop) const {
  return m_hid.Square(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Cross(frc::EventLoop* loop) const {
  return m_hid.Cross(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Circle(frc::EventLoop* loop) const {
  return m_hid.Circle(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Triangle(frc::EventLoop* loop) const {
  return m_hid.Triangle(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::L1(frc::EventLoop* loop) const {
  return m_hid.L1(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::R1(frc::EventLoop* loop) const {
  return m_hid.R1(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::L2(frc::EventLoop* loop) const {
  return m_hid.L2(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::R2(frc::EventLoop* loop) const {
  return m_hid.R2(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Options(frc::EventLoop* loop) const {
  return m_hid.Options(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::L3(frc::EventLoop* loop) const {
  return m_hid.L3(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::R3(frc::EventLoop* loop) const {
  return m_hid.R3(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::PS(frc::EventLoop* loop) const {
  return m_hid.PS(loop).CastTo<Trigger>();
}

Trigger CommandPS5Controller::Touchpad(frc::EventLoop* loop) const {
  return m_hid.Touchpad(loop).CastTo<Trigger>();
}
