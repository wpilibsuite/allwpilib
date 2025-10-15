// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/CommandGenericHID.hpp"

using namespace frc2;

CommandGenericHID::CommandGenericHID(int port) : m_hid{port} {}

frc::GenericHID& CommandGenericHID::GetHID() {
  return m_hid;
}

Trigger CommandGenericHID::Button(int button, frc::EventLoop* loop) const {
  return Trigger(loop, [this, button] { return m_hid.GetRawButton(button); });
}

Trigger CommandGenericHID::POV(frc::DriverStation::POVDirection angle,
                               frc::EventLoop* loop) const {
  return POV(0, angle, loop);
}

Trigger CommandGenericHID::POV(int pov, frc::DriverStation::POVDirection angle,
                               frc::EventLoop* loop) const {
  return Trigger(loop,
                 [this, pov, angle] { return m_hid.GetPOV(pov) == angle; });
}

Trigger CommandGenericHID::POVUp(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kUp, loop);
}

Trigger CommandGenericHID::POVUpRight(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kUpRight, loop);
}

Trigger CommandGenericHID::POVRight(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kRight, loop);
}

Trigger CommandGenericHID::POVDownRight(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kDownRight, loop);
}

Trigger CommandGenericHID::POVDown(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kDown, loop);
}

Trigger CommandGenericHID::POVDownLeft(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kDownLeft, loop);
}

Trigger CommandGenericHID::POVLeft(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kLeft, loop);
}

Trigger CommandGenericHID::POVUpLeft(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kUpLeft, loop);
}

Trigger CommandGenericHID::POVCenter(frc::EventLoop* loop) const {
  return POV(frc::DriverStation::POVDirection::kCenter, loop);
}

Trigger CommandGenericHID::AxisLessThan(int axis, double threshold,
                                        frc::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return m_hid.GetRawAxis(axis) < threshold;
  });
}

Trigger CommandGenericHID::AxisGreaterThan(int axis, double threshold,
                                           frc::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return m_hid.GetRawAxis(axis) > threshold;
  });
}

Trigger CommandGenericHID::AxisMagnitudeGreaterThan(
    int axis, double threshold, frc::EventLoop* loop) const {
  return Trigger(loop, [this, axis, threshold]() {
    return std::abs(m_hid.GetRawAxis(axis)) > threshold;
  });
}

void CommandGenericHID::SetRumble(frc::GenericHID::RumbleType type,
                                  double value) {
  m_hid.SetRumble(type, value);
}

bool CommandGenericHID::IsConnected() const {
  return m_hid.IsConnected();
}
