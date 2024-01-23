// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandXboxController.h"

using namespace frc2;

CommandXboxController::CommandXboxController(int port)
    : CommandGenericHID(port), m_hid{frc::XboxController(port)} {}

frc::XboxController& CommandXboxController::GetHID() {
  return m_hid;
}

Trigger CommandXboxController::LeftBumper(frc::EventLoop* loop) const {
  return m_hid.LeftBumper(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::RightBumper(frc::EventLoop* loop) const {
  return m_hid.RightBumper(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::LeftStick(frc::EventLoop* loop) const {
  return m_hid.LeftStick(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::RightStick(frc::EventLoop* loop) const {
  return m_hid.RightStick(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::A(frc::EventLoop* loop) const {
  return m_hid.A(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::B(frc::EventLoop* loop) const {
  return m_hid.B(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::X(frc::EventLoop* loop) const {
  return m_hid.X(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Y(frc::EventLoop* loop) const {
  return m_hid.Y(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Back(frc::EventLoop* loop) const {
  return m_hid.Back(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::Start(frc::EventLoop* loop) const {
  return m_hid.Start(loop).CastTo<Trigger>();
}

Trigger CommandXboxController::LeftTrigger(double threshold,
                                           frc::EventLoop* loop) const {
  return m_hid.LeftTrigger(threshold, loop).CastTo<Trigger>();
}

Trigger CommandXboxController::RightTrigger(double threshold,
                                            frc::EventLoop* loop) const {
  return m_hid.RightTrigger(threshold, loop).CastTo<Trigger>();
}

double CommandXboxController::GetRightTriggerAxis() {
  return m_hid.GetRightTriggerAxis();
}

double CommandXboxController::GetLeftTriggerAxis() {
  return m_hid.GetLeftTriggerAxis();
}

double CommandXboxController::GetRightY() {
  return m_hid.GetRightY();
}

double CommandXboxController::GetLeftY() {
  return m_hid.GetLeftY();
}

double CommandXboxController::GetRightX() {
  return m_hid.GetRightX();
}

double CommandXboxController::GetLeftX() {
  return m_hid.GetLeftX();
}
