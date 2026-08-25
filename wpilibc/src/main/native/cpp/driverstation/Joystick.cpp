// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Joystick.hpp"

#include <cmath>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/event/BooleanEvent.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi;

Joystick::Joystick(int port) : Joystick{DriverStation::GetGenericHID(port)} {}

Joystick::Joystick(GenericHID& hid) : m_hid{&hid} {
  m_axes[Axis::X] = DEFAULT_X_CHANNEL;
  m_axes[Axis::Y] = DEFAULT_Y_CHANNEL;
  m_axes[Axis::Z] = DEFAULT_Z_CHANNEL;
  m_axes[Axis::TWIST] = DEFAULT_TWIST_CHANNEL;
  m_axes[Axis::THROTTLE] = DEFAULT_THROTTLE_CHANNEL;

  wpi::util::ReportUsage("HID", hid.GetPort(), "Joystick");
}

GenericHID& Joystick::GetHID() {
  return *m_hid;
}

const GenericHID& Joystick::GetHID() const {
  return *m_hid;
}

void Joystick::SetXChannel(int channel) {
  m_axes[Axis::X] = channel;
}

void Joystick::SetYChannel(int channel) {
  m_axes[Axis::Y] = channel;
}

void Joystick::SetZChannel(int channel) {
  m_axes[Axis::Z] = channel;
}

void Joystick::SetTwistChannel(int channel) {
  m_axes[Axis::TWIST] = channel;
}

void Joystick::SetThrottleChannel(int channel) {
  m_axes[Axis::THROTTLE] = channel;
}

int Joystick::GetXChannel() const {
  return m_axes[Axis::X];
}

int Joystick::GetYChannel() const {
  return m_axes[Axis::Y];
}

int Joystick::GetZChannel() const {
  return m_axes[Axis::Z];
}

int Joystick::GetTwistChannel() const {
  return m_axes[Axis::TWIST];
}

int Joystick::GetThrottleChannel() const {
  return m_axes[Axis::THROTTLE];
}

bool Joystick::GetRawButton(int button) const {
  return m_hid->GetRawButton(button);
}

bool Joystick::GetRawButtonPressed(int button) {
  return m_hid->GetRawButtonPressed(button);
}

bool Joystick::GetRawButtonReleased(int button) {
  return m_hid->GetRawButtonReleased(button);
}

double Joystick::GetRawAxis(int axis) const {
  return m_hid->GetRawAxis(axis);
}

POVDirection Joystick::GetPOV(int pov) const {
  return m_hid->GetPOV(pov);
}

double Joystick::GetX() const {
  return m_hid->GetRawAxis(m_axes[Axis::X]);
}

double Joystick::GetY() const {
  return m_hid->GetRawAxis(m_axes[Axis::Y]);
}

double Joystick::GetZ() const {
  return m_hid->GetRawAxis(m_axes[Axis::Z]);
}

double Joystick::GetTwist() const {
  return m_hid->GetRawAxis(m_axes[Axis::TWIST]);
}

double Joystick::GetThrottle() const {
  return m_hid->GetRawAxis(m_axes[Axis::THROTTLE]);
}

bool Joystick::GetTrigger() const {
  return m_hid->GetRawButton(Button::TRIGGER);
}

bool Joystick::GetTriggerPressed() {
  return m_hid->GetRawButtonPressed(Button::TRIGGER);
}

bool Joystick::GetTriggerReleased() {
  return m_hid->GetRawButtonReleased(Button::TRIGGER);
}

BooleanEvent Joystick::Trigger(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetTrigger(); });
}

bool Joystick::GetTop() const {
  return m_hid->GetRawButton(Button::TOP);
}

bool Joystick::GetTopPressed() {
  return m_hid->GetRawButtonPressed(Button::TOP);
}

bool Joystick::GetTopReleased() {
  return m_hid->GetRawButtonReleased(Button::TOP);
}

BooleanEvent Joystick::Top(EventLoop* loop) const {
  return BooleanEvent(loop, [this]() { return this->GetTop(); });
}

double Joystick::GetMagnitude() const {
  return std::hypot(GetX(), GetY());
}

wpi::units::radians<> Joystick::GetDirection() const {
  // https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html#joystick-and-controller-coordinate-system
  // A positive rotation around the X axis moves the joystick right, and a
  // positive rotation around the Y axis moves the joystick backward. When
  // treating them as translations, 0 radians is measured from the right
  // direction, and angle increases clockwise.
  //
  // It's rotated 90 degrees CCW (y is negated and the arguments are reversed)
  // so that 0 radians is forward.
  return wpi::units::radians<>{std::atan2(GetX(), -GetY())};
}
