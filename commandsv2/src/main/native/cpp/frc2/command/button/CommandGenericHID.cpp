// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandGenericHID.hpp"

using namespace wpi::cmd;

CommandGenericHID::CommandGenericHID(int port) : m_hid{port} {}

wpi::GenericHID& CommandGenericHID::GetHID() {
  return m_hid;
}

Trigger CommandGenericHID::Button(int button, wpi::EventLoop* loop) const {
  return Trigger(loop, [this, button] { return m_hid.GetRawButton(button); });
}

Trigger CommandGenericHID::POV(wpi::POVDirection angle,
                               wpi::EventLoop* loop) const {
  return POV(0, angle, loop);
}

Trigger CommandGenericHID::POV(int pov, wpi::POVDirection angle,
                               wpi::EventLoop* loop) const {
  return Trigger(loop,
                 [this, pov, angle] { return m_hid.GetPOV(pov) == angle; });
}

Trigger CommandGenericHID::POVUp(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kUp, loop);
}

Trigger CommandGenericHID::POVUpRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kUpRight, loop);
}

Trigger CommandGenericHID::POVRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kRight, loop);
}

Trigger CommandGenericHID::POVDownRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kDownRight, loop);
}

Trigger CommandGenericHID::POVDown(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kDown, loop);
}

Trigger CommandGenericHID::POVDownLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kDownLeft, loop);
}

Trigger CommandGenericHID::POVLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kLeft, loop);
}

Trigger CommandGenericHID::POVUpLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kUpLeft, loop);
}

Trigger CommandGenericHID::POVCenter(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::kCenter, loop);
}

Trigger CommandGenericHID::AxisLessThan(int axis, double threshold,
                                        wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return m_hid.GetRawAxis(axis) < threshold;
  });
}

Trigger CommandGenericHID::AxisGreaterThan(int axis, double threshold,
                                           wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return m_hid.GetRawAxis(axis) > threshold;
  });
}

Trigger CommandGenericHID::AxisMagnitudeGreaterThan(
    int axis, double threshold, wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return std::abs(m_hid.GetRawAxis(axis)) > threshold;
  });
}

void CommandGenericHID::SetRumble(wpi::GenericHID::RumbleType type,
                                  double value) {
  m_hid.SetRumble(type, value);
}

bool CommandGenericHID::IsConnected() const {
  return m_hid.IsConnected();
}
