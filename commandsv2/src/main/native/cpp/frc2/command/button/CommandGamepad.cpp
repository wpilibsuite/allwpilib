// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandGamepad.hpp"

using namespace wpi::cmd;

CommandGamepad::CommandGamepad(int port)
    : CommandGenericHID(port), m_hid{wpi::Gamepad(port)} {}

wpi::Gamepad& CommandGamepad::GetHID() {
  return m_hid;
}

Trigger CommandGamepad::SouthFace(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kSouthFace, loop);
}

Trigger CommandGamepad::EastFace(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kEastFace, loop);
}

Trigger CommandGamepad::WestFace(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kWestFace, loop);
}

Trigger CommandGamepad::NorthFace(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kNorthFace, loop);
}

Trigger CommandGamepad::Back(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kBack, loop);
}

Trigger CommandGamepad::Guide(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kGuide, loop);
}

Trigger CommandGamepad::Start(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kStart, loop);
}

Trigger CommandGamepad::LeftStick(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kLeftStick, loop);
}

Trigger CommandGamepad::RightStick(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kRightStick, loop);
}

Trigger CommandGamepad::LeftShoulder(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kLeftShoulder, loop);
}

Trigger CommandGamepad::RightShoulder(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kRightShoulder, loop);
}

Trigger CommandGamepad::DpadUp(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kDpadUp, loop);
}

Trigger CommandGamepad::DpadDown(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kDpadDown, loop);
}

Trigger CommandGamepad::DpadLeft(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kDpadLeft, loop);
}

Trigger CommandGamepad::DpadRight(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kDpadRight, loop);
}

Trigger CommandGamepad::Misc1(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kMisc1, loop);
}

Trigger CommandGamepad::RightPaddle1(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kRightPaddle1, loop);
}

Trigger CommandGamepad::LeftPaddle1(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kLeftPaddle1, loop);
}

Trigger CommandGamepad::RightPaddle2(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kRightPaddle2, loop);
}

Trigger CommandGamepad::LeftPaddle2(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kLeftPaddle2, loop);
}

Trigger CommandGamepad::Touchpad(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kTouchpad, loop);
}

Trigger CommandGamepad::Misc2(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kMisc2, loop);
}

Trigger CommandGamepad::Misc3(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kMisc3, loop);
}

Trigger CommandGamepad::Misc4(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kMisc4, loop);
}

Trigger CommandGamepad::Misc5(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kMisc5, loop);
}

Trigger CommandGamepad::Misc6(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::kMisc6, loop);
}

Trigger CommandGamepad::LeftTrigger(double threshold,
                                    wpi::EventLoop* loop) const {
  return Trigger(loop, [this, threshold] {
    return m_hid.GetLeftTriggerAxis() > threshold;
  });
}

Trigger CommandGamepad::RightTrigger(double threshold,
                                     wpi::EventLoop* loop) const {
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
