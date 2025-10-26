// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/CommandGamepad.h"

using namespace frc2;

CommandGamepad::CommandGamepad(int port)
    : CommandGenericHID(port), m_hid{frc::Gamepad(port)} {}

frc::Gamepad& CommandGamepad::GetHID() {
  return m_hid;
}

Trigger CommandGamepad::SouthFace(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kSouthFace, loop);
}

Trigger CommandGamepad::EastFace(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kEastFace, loop);
}

Trigger CommandGamepad::WestFacen(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kWestFacen, loop);
}

Trigger CommandGamepad::NorthFacen(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kNorthFacen, loop);
}

Trigger CommandGamepad::Back(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kBack, loop);
}

Trigger CommandGamepad::Guide(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kGuide, loop);
}

Trigger CommandGamepad::Start(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kStart, loop);
}

Trigger CommandGamepad::LeftStick(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kLeftStick, loop);
}

Trigger CommandGamepad::RightStick(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kRightStick, loop);
}

Trigger CommandGamepad::LeftShoulder(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kLeftShoulder, loop);
}

Trigger CommandGamepad::RightShoulder(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kRightShoulder, loop);
}

Trigger CommandGamepad::DpadUp(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kDpadUp, loop);
}

Trigger CommandGamepad::DpadDown(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kDpadDown, loop);
}

Trigger CommandGamepad::DpadLeft(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kDpadLeft, loop);
}

Trigger CommandGamepad::DpadRight(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kDpadRight, loop);
}

Trigger CommandGamepad::Misc1(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kMisc1, loop);
}

Trigger CommandGamepad::RightPaddle1(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kRightPaddle1, loop);
}

Trigger CommandGamepad::LeftPaddle1(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kLeftPaddle1, loop);
}

Trigger CommandGamepad::RightPaddle2(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kRightPaddle2, loop);
}

Trigger CommandGamepad::LeftPaddle2(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kLeftPaddle2, loop);
}

Trigger CommandGamepad::Touchpad(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kTouchpad, loop);
}

Trigger CommandGamepad::Misc2(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kMisc2, loop);
}

Trigger CommandGamepad::Misc3(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kMisc3, loop);
}

Trigger CommandGamepad::Misc4(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kMisc4, loop);
}

Trigger CommandGamepad::Misc5(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kMisc5, loop);
}

Trigger CommandGamepad::Misc6(frc::EventLoop* loop) const {
  return Button(frc::Gamepad::Button::kMisc6, loop);
}

Trigger CommandGamepad::LeftTrigger(double threshold,
                                           frc::EventLoop* loop) const {
  return Trigger(loop, [this, threshold] {
    return m_hid.GetLeftTriggerAxis() > threshold;
  });
}

Trigger CommandGamepad::RightTrigger(double threshold,
                                           frc::EventLoop* loop) const {
  return Trigger(loop, [this, threshold] {
    return m_hid.GetRightTriggerAxis() > threshold;
  });
}

double CommandGamepad::GetLeftX() const {
  return m_hid.GetLeftX();
}

double CommandGamepad::GetLeftY() const {
  return m_hid.GetLeftY();
}

double CommandGamepad::GetRightX() const {
  return m_hid.GetRightX();
}

double CommandGamepad::GetRightY() const {
  return m_hid.GetRightY();
}

double CommandGamepad::GetLeftTriggerAxis() const {
  return m_hid.GetLeftTriggerAxis();
}

double CommandGamepad::GetRightTriggerAxis() const {
  return m_hid.GetRightTriggerAxis();
}
