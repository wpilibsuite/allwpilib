// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandGamepad.hpp"

#include <memory>

#include "wpi/commands2/button/CommandGenericHID.hpp"

using namespace wpi::cmd;

CommandGamepad::CommandGamepad(int port)
    : m_hid{&CommandGenericHID::GetCommandGenericHID(port)},
      m_gamepad{&wpi::DriverStation::GetGamepad(port)} {}

CommandGamepad::CommandGamepad(wpi::Gamepad* gamepad)
    : m_ownedHid{std::make_unique<CommandGenericHID>(gamepad->GetHID())},
      m_hid{m_ownedHid.get()},
      m_gamepad{gamepad} {}

CommandGenericHID& CommandGamepad::GetHID() {
  return *m_hid;
}

wpi::Gamepad& CommandGamepad::GetGamepad() {
  return *m_gamepad;
}

Trigger CommandGamepad::Button(int button, wpi::EventLoop* loop) const {
  return m_hid->Button(button, loop);
}

Trigger CommandGamepad::Button(enum wpi::Gamepad::Button button,
                               wpi::EventLoop* loop) const {
  return Button(static_cast<int>(button), loop);
}

Trigger CommandGamepad::FaceDown(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::FACE_DOWN, loop);
}

Trigger CommandGamepad::FaceRight(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::FACE_RIGHT, loop);
}

Trigger CommandGamepad::FaceLeft(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::FACE_LEFT, loop);
}

Trigger CommandGamepad::FaceUp(wpi::EventLoop* loop) const {
  return Button(wpi::Gamepad::Button::FACE_UP, loop);
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
    return m_gamepad->GetAxis(wpi::Gamepad::Axis::LEFT_TRIGGER) > threshold;
  });
}

Trigger CommandGamepad::RightTrigger(double threshold,
                                     wpi::EventLoop* loop) const {
  return Trigger(loop, [this, threshold] {
    return m_gamepad->GetAxis(wpi::Gamepad::Axis::RIGHT_TRIGGER) > threshold;
  });
}

Trigger CommandGamepad::AxisLessThan(wpi::Gamepad::Axis axis, double threshold,
                                     wpi::EventLoop* loop) const {
  return m_hid->AxisLessThan(static_cast<int>(axis), threshold, loop);
}

Trigger CommandGamepad::AxisGreaterThan(wpi::Gamepad::Axis axis,
                                        double threshold,
                                        wpi::EventLoop* loop) const {
  return m_hid->AxisGreaterThan(static_cast<int>(axis), threshold, loop);
}

Trigger CommandGamepad::AxisMagnitudeGreaterThan(wpi::Gamepad::Axis axis,
                                                 double threshold,
                                                 wpi::EventLoop* loop) const {
  return m_hid->AxisMagnitudeGreaterThan(static_cast<int>(axis), threshold,
                                         loop);
}

double CommandGamepad::GetLeftX() const {
  return m_gamepad->GetLeftX();
}

double CommandGamepad::GetLeftY() const {
  return m_gamepad->GetLeftY();
}

double CommandGamepad::GetRightX() const {
  return m_gamepad->GetRightX();
}

double CommandGamepad::GetRightY() const {
  return m_gamepad->GetRightY();
}

double CommandGamepad::GetLeftTrigger() const {
  return m_gamepad->GetLeftTrigger();
}

double CommandGamepad::GetRightTrigger() const {
  return m_gamepad->GetRightTrigger();
}

CommandPtr CommandGamepad::RumbleLeft(double value) {
  return m_hid->RumbleLeft(value);
}

CommandPtr CommandGamepad::RumbleRight(double value) {
  return m_hid->RumbleRight(value);
}

CommandPtr CommandGamepad::RumbleBoth(double value) {
  return m_hid->RumbleBoth(value);
}

CommandPtr CommandGamepad::RumbleLeftTrigger(double value) {
  return m_hid->RumbleLeftTrigger(value);
}

CommandPtr CommandGamepad::RumbleRightTrigger(double value) {
  return m_hid->RumbleRightTrigger(value);
}

CommandPtr CommandGamepad::RumbleTriggers(double value) {
  return m_hid->RumbleTriggers(value);
}

CommandPtr CommandGamepad::SetLeds(int r, int g, int b) const {
  return m_hid->SetLeds(r, g, b);
}

CommandPtr CommandGamepad::SetLeds(const util::Color& color) const {
  return m_hid->SetLeds(color);
}

CommandPtr CommandGamepad::SetLeds(const util::Color8Bit& color) const {
  return m_hid->SetLeds(color);
}
