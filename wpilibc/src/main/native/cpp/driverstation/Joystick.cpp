// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Joystick.hpp"

#include <cmath>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/event/BooleanEvent.hpp"
#include "wpi/hal/UsageReporting.hpp"

using namespace wpi;

Joystick::Joystick(int port) : Joystick{DriverStation::GetGenericHID(port)} {}

Joystick::Joystick(GenericHID& hid) : m_hid{&hid} {
  m_axes[Axis::kX] = kDefaultXChannel;
  m_axes[Axis::kY] = kDefaultYChannel;
  m_axes[Axis::kZ] = kDefaultZChannel;
  m_axes[Axis::kTwist] = kDefaultTwistChannel;
  m_axes[Axis::kThrottle] = kDefaultThrottleChannel;

  HAL_ReportUsage("HID", hid.GetPort(), "Joystick");
}

GenericHID& Joystick::GetHID() {
  return *m_hid;
}

const GenericHID& Joystick::GetHID() const {
  return *m_hid;
}

void Joystick::SetXChannel(int channel) {
  m_axes[Axis::kX] = channel;
}

void Joystick::SetYChannel(int channel) {
  m_axes[Axis::kY] = channel;
}

void Joystick::SetZChannel(int channel) {
  m_axes[Axis::kZ] = channel;
}

void Joystick::SetTwistChannel(int channel) {
  m_axes[Axis::kTwist] = channel;
}

void Joystick::SetThrottleChannel(int channel) {
  m_axes[Axis::kThrottle] = channel;
}

int Joystick::GetXChannel() const {
  return m_axes[Axis::kX];
}

int Joystick::GetYChannel() const {
  return m_axes[Axis::kY];
}

int Joystick::GetZChannel() const {
  return m_axes[Axis::kZ];
}

int Joystick::GetTwistChannel() const {
  return m_axes[Axis::kTwist];
}

int Joystick::GetThrottleChannel() const {
  return m_axes[Axis::kThrottle];
}

double Joystick::GetX() const {
  return m_hid->GetRawAxis(m_axes[Axis::kX]);
}

double Joystick::GetY() const {
  return m_hid->GetRawAxis(m_axes[Axis::kY]);
}

double Joystick::GetZ() const {
  return m_hid->GetRawAxis(m_axes[Axis::kZ]);
}

double Joystick::GetTwist() const {
  return m_hid->GetRawAxis(m_axes[Axis::kTwist]);
}

double Joystick::GetThrottle() const {
  return m_hid->GetRawAxis(m_axes[Axis::kThrottle]);
}

bool Joystick::GetTrigger() const {
  return m_hid->GetRawButton(Button::kTrigger);
}

bool Joystick::GetTriggerPressed() {
  return m_hid->GetRawButtonPressed(Button::kTrigger);
}

bool Joystick::GetTriggerReleased() {
  return m_hid->GetRawButtonReleased(Button::kTrigger);
}

BooleanEvent Joystick::Trigger(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetTrigger(); });
}

bool Joystick::GetTop() const {
  return m_hid->GetRawButton(Button::kTop);
}

bool Joystick::GetTopPressed() {
  return m_hid->GetRawButtonPressed(Button::kTop);
}

bool Joystick::GetTopReleased() {
  return m_hid->GetRawButtonReleased(Button::kTop);
}

BooleanEvent Joystick::Top(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetTop(); });
}

double Joystick::GetMagnitude() const {
  return std::hypot(GetX(), GetY());
}

wpi::units::radian_t Joystick::GetDirection() const {
  // https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html#joystick-and-controller-coordinate-system
  // A positive rotation around the X axis moves the joystick right, and a
  // positive rotation around the Y axis moves the joystick backward. When
  // treating them as translations, 0 radians is measured from the right
  // direction, and angle increases clockwise.
  //
  // It's rotated 90 degrees CCW (y is negated and the arguments are reversed)
  // so that 0 radians is forward.
  return wpi::units::radian_t{std::atan2(GetX(), -GetY())};
}
