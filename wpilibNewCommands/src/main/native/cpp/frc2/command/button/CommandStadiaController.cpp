// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandStadiaController.h"

using namespace frc2;

CommandStadiaController::CommandStadiaController(int port)
    : CommandGenericHID(port), m_hid{frc::StadiaController(port)} {}

frc::StadiaController& CommandStadiaController::GetHID() {
  return m_hid;
}

Trigger CommandStadiaController::LeftBumper(frc::EventLoop* loop) const {
  return m_hid.LeftBumper(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::RightBumper(frc::EventLoop* loop) const {
  return m_hid.RightBumper(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::LeftStick(frc::EventLoop* loop) const {
  return m_hid.LeftStick(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::RightStick(frc::EventLoop* loop) const {
  return m_hid.RightStick(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::A(frc::EventLoop* loop) const {
  return m_hid.A(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::B(frc::EventLoop* loop) const {
  return m_hid.B(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::X(frc::EventLoop* loop) const {
  return m_hid.X(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Y(frc::EventLoop* loop) const {
  return m_hid.Y(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Ellipses(frc::EventLoop* loop) const {
  return m_hid.Ellipses(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Hamburger(frc::EventLoop* loop) const {
  return m_hid.Hamburger(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Stadia(frc::EventLoop* loop) const {
  return m_hid.Stadia(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Google(frc::EventLoop* loop) const {
  return m_hid.Google(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::Frame(frc::EventLoop* loop) const {
  return m_hid.Frame(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::LeftTrigger(frc::EventLoop* loop) const {
  return m_hid.LeftTrigger(loop).CastTo<Trigger>();
}

Trigger CommandStadiaController::RightTrigger(frc::EventLoop* loop) const {
  return m_hid.RightTrigger(loop).CastTo<Trigger>();
}

double CommandStadiaController::GetLeftX() const {
  return m_hid.GetLeftX();
}

double CommandStadiaController::GetRightX() const {
  return m_hid.GetRightX();
}

double CommandStadiaController::GetLeftY() const {
  return m_hid.GetLeftY();
}

double CommandStadiaController::GetRightY() const {
  return m_hid.GetRightY();
}
