// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandGenericHID.hpp"

#include <array>
#include <memory>

using namespace wpi::cmd;

namespace {
std::array<std::unique_ptr<CommandGenericHID>,
           wpi::internal::DriverStationBackend::JOYSTICK_PORTS>
    hids;
}  // namespace

CommandGenericHID::CommandGenericHID(int port)
    : CommandGenericHID{wpi::DriverStation::GetGenericHID(port)} {}

CommandGenericHID::CommandGenericHID(wpi::GenericHID& hid) : m_hid{&hid} {}

CommandGenericHID& CommandGenericHID::GetCommandGenericHID(int port) {
  wpi::DriverStation::GetGenericHID(port);
  if (!hids[port]) {
    hids[port] = std::make_unique<CommandGenericHID>(port);
  }

  return *hids[port];
}

wpi::GenericHID& CommandGenericHID::GetHID() {
  return *m_hid;
}

Trigger CommandGenericHID::Button(int button, wpi::EventLoop* loop) const {
  return Trigger(loop, [this, button] { return m_hid->GetRawButton(button); });
}

Trigger CommandGenericHID::POV(wpi::POVDirection angle,
                               wpi::EventLoop* loop) const {
  return POV(0, angle, loop);
}

Trigger CommandGenericHID::POV(int pov, wpi::POVDirection angle,
                               wpi::EventLoop* loop) const {
  return Trigger(loop,
                 [this, pov, angle] { return m_hid->GetPOV(pov) == angle; });
}

Trigger CommandGenericHID::POVUp(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::UP, loop);
}

Trigger CommandGenericHID::POVUpRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::UP_RIGHT, loop);
}

Trigger CommandGenericHID::POVRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::RIGHT, loop);
}

Trigger CommandGenericHID::POVDownRight(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::DOWN_RIGHT, loop);
}

Trigger CommandGenericHID::POVDown(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::DOWN, loop);
}

Trigger CommandGenericHID::POVDownLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::DOWN_LEFT, loop);
}

Trigger CommandGenericHID::POVLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::LEFT, loop);
}

Trigger CommandGenericHID::POVUpLeft(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::UP_LEFT, loop);
}

Trigger CommandGenericHID::POVCenter(wpi::EventLoop* loop) const {
  return POV(wpi::POVDirection::CENTER, loop);
}

Trigger CommandGenericHID::AxisLessThan(int axis, double threshold,
                                        wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return m_hid->GetRawAxis(axis) < threshold;
  });
}

Trigger CommandGenericHID::AxisGreaterThan(int axis, double threshold,
                                           wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return m_hid->GetRawAxis(axis) > threshold;
  });
}

Trigger CommandGenericHID::AxisMagnitudeGreaterThan(
    int axis, double threshold, wpi::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return std::abs(m_hid->GetRawAxis(axis)) > threshold;
  });
}

void CommandGenericHID::SetRumble(wpi::GenericHID::RumbleType type,
                                  double value) {
  m_hid->SetRumble(type, value);
}

bool CommandGenericHID::IsConnected() const {
  return m_hid->IsConnected();
}
