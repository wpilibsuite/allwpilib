// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandPS4Controller.h"

using namespace frc2;

CommandPS4Controller::CommandPS4Controller(int port)
    : CommandGenericHID(port), m_hid{frc::PS4Controller(port)} {}

frc::PS4Controller& CommandPS4Controller::GetHID() {
  return m_hid;
}

Trigger CommandPS4Controller::Square(frc::EventLoop* loop) const {
  return m_hid.Square(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Cross(frc::EventLoop* loop) const {
  return m_hid.Cross(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Circle(frc::EventLoop* loop) const {
  return m_hid.Circle(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Triangle(frc::EventLoop* loop) const {
  return m_hid.Triangle(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L1(frc::EventLoop* loop) const {
  return m_hid.L1(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R1(frc::EventLoop* loop) const {
  return m_hid.R1(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L2(frc::EventLoop* loop) const {
  return m_hid.L2(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R2(frc::EventLoop* loop) const {
  return m_hid.R2(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Options(frc::EventLoop* loop) const {
  return m_hid.Options(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::L3(frc::EventLoop* loop) const {
  return m_hid.L3(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::R3(frc::EventLoop* loop) const {
  return m_hid.R3(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::PS(frc::EventLoop* loop) const {
  return m_hid.PS(loop).CastTo<Trigger>();
}

Trigger CommandPS4Controller::Touchpad(frc::EventLoop* loop) const {
  return m_hid.Touchpad(loop).CastTo<Trigger>();
}

double CommandPS4Controller::GetR2Axis() {
  return m_hid.GetR2Axis();
}

double CommandPS4Controller::GetL2Axis() {
  return m_hid.GetL2Axis();
}

double CommandPS4Controller::GetRightY() {
  return m_hid.GetRightY();
}

double CommandPS4Controller::GetLeftY() {
  return m_hid.GetLeftY();
}

double CommandPS4Controller::GetRightX() {
  return m_hid.GetRightX();
}

double CommandPS4Controller::GetLeftX() {
  return m_hid.GetLeftX();
}
