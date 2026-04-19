// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandGamepad.hpp"

#include "wpi/commands2/button/CommandGenericHID.hpp"

using namespace wpi::cmd;

CommandGamepad::CommandGamepad(int port)
    : CommandGenericHID(port), m_hid{wpi::Gamepad(port)} {}

wpi::Gamepad& CommandGamepad::GetHID() {
  return m_hid;
}

Trigger CommandGamepad::Button(enum wpi::Gamepad::Button button,
                               wpi::EventLoop* loop) const {
  return CommandGenericHID::Button(static_cast<int>(button), loop);
}

Trigger CommandGamepad::SouthFace(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::SOUTH_FACE, loop);
}

Trigger CommandGamepad::EastFace(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::EAST_FACE, loop);
}

Trigger CommandGamepad::WestFace(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::WEST_FACE, loop);
}

Trigger CommandGamepad::NorthFace(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::NORTH_FACE, loop);
}

Trigger CommandGamepad::Back(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::BACK, loop);
}

Trigger CommandGamepad::Guide(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::GUIDE, loop);
}

Trigger CommandGamepad::Start(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::START, loop);
}

Trigger CommandGamepad::LeftStick(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::LEFT_STICK, loop);
}

Trigger CommandGamepad::RightStick(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::RIGHT_STICK, loop);
}

Trigger CommandGamepad::LeftBumper(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::LEFT_BUMPER, loop);
}

Trigger CommandGamepad::RightBumper(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::RIGHT_BUMPER, loop);
}

Trigger CommandGamepad::DpadUp(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::DPAD_UP, loop);
}

Trigger CommandGamepad::DpadDown(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::DPAD_DOWN, loop);
}

Trigger CommandGamepad::DpadLeft(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::DPAD_LEFT, loop);
}

Trigger CommandGamepad::DpadRight(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::DPAD_RIGHT, loop);
}

Trigger CommandGamepad::Misc1(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::MISC_1, loop);
}

Trigger CommandGamepad::RightPaddle1(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::RIGHT_PADDLE_1, loop);
}

Trigger CommandGamepad::LeftPaddle1(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::LEFT_PADDLE_1, loop);
}

Trigger CommandGamepad::RightPaddle2(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::RIGHT_PADDLE_2, loop);
}

Trigger CommandGamepad::LeftPaddle2(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::LEFT_PADDLE_2, loop);
}

Trigger CommandGamepad::Touchpad(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::TOUCHPAD, loop);
}

Trigger CommandGamepad::Misc2(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::MISC_2, loop);
}

Trigger CommandGamepad::Misc3(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::MISC_3, loop);
}

Trigger CommandGamepad::Misc4(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::MISC_4, loop);
}

Trigger CommandGamepad::Misc5(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::MISC_5, loop);
}

Trigger CommandGamepad::Misc6(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::MISC_6, loop);
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

Trigger CommandGamepad::AxisLessThan(wpi::Gamepad::Axis axis, double threshold,
                                     wpi::EventLoop* loop) const {
  return CommandGenericHID::AxisLessThan(static_cast<int>(axis), threshold,
                                         loop);
}

Trigger CommandGamepad::AxisGreaterThan(wpi::Gamepad::Axis axis,
                                        double threshold,
                                        wpi::EventLoop* loop) const {
  return CommandGenericHID::AxisGreaterThan(static_cast<int>(axis), threshold,
                                            loop);
}

Trigger CommandGamepad::AxisMagnitudeGreaterThan(wpi::Gamepad::Axis axis,
                                                 double threshold,
                                                 wpi::EventLoop* loop) const {
  return CommandGenericHID::AxisMagnitudeGreaterThan(static_cast<int>(axis),
                                                     threshold, loop);
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
