// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/GenericHID.hpp"

#include <string>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/event/BooleanEvent.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/system/Errors.hpp"

using namespace wpi;

GenericHID::GenericHID(int port) {
  if (port < 0 || port >= DriverStation::kJoystickPorts) {
    throw WPILIB_MakeError(warn::BadJoystickIndex, "port {} out of range",
                           port);
  }
  m_port = port;
}

bool GenericHID::GetRawButton(int button) const {
  return DriverStation::GetStickButton(m_port, button);
}

bool GenericHID::GetRawButtonPressed(int button) {
  return DriverStation::GetStickButtonPressed(m_port, button);
}

bool GenericHID::GetRawButtonReleased(int button) {
  return DriverStation::GetStickButtonReleased(m_port, button);
}

BooleanEvent GenericHID::Button(int button, EventLoop* loop) const {
  return BooleanEvent(loop,
                      [this, button]() { return this->GetRawButton(button); });
}

double GenericHID::GetRawAxis(int axis) const {
  return DriverStation::GetStickAxis(m_port, axis);
}

DriverStation::POVDirection GenericHID::GetPOV(int pov) const {
  return DriverStation::GetStickPOV(m_port, pov);
}

BooleanEvent GenericHID::POV(DriverStation::POVDirection angle,
                             EventLoop* loop) const {
  return POV(0, angle, loop);
}

BooleanEvent GenericHID::POV(int pov, DriverStation::POVDirection angle,
                             EventLoop* loop) const {
  return BooleanEvent(
      loop, [this, pov, angle] { return this->GetPOV(pov) == angle; });
}

BooleanEvent GenericHID::POVUp(EventLoop* loop) const {
  return POV(DriverStation::kUp, loop);
}

BooleanEvent GenericHID::POVUpRight(EventLoop* loop) const {
  return POV(DriverStation::kUpRight, loop);
}

BooleanEvent GenericHID::POVRight(EventLoop* loop) const {
  return POV(DriverStation::kRight, loop);
}

BooleanEvent GenericHID::POVDownRight(EventLoop* loop) const {
  return POV(DriverStation::kDownRight, loop);
}

BooleanEvent GenericHID::POVDown(EventLoop* loop) const {
  return POV(DriverStation::kDown, loop);
}

BooleanEvent GenericHID::POVDownLeft(EventLoop* loop) const {
  return POV(DriverStation::kDownLeft, loop);
}

BooleanEvent GenericHID::POVLeft(EventLoop* loop) const {
  return POV(DriverStation::kLeft, loop);
}

BooleanEvent GenericHID::POVUpLeft(EventLoop* loop) const {
  return POV(DriverStation::kUpLeft, loop);
}

BooleanEvent GenericHID::POVCenter(EventLoop* loop) const {
  return POV(DriverStation::kCenter, loop);
}

BooleanEvent GenericHID::AxisLessThan(int axis, double threshold,
                                      EventLoop* loop) const {
  return BooleanEvent(loop, [this, axis, threshold]() {
    return this->GetRawAxis(axis) < threshold;
  });
}

BooleanEvent GenericHID::AxisGreaterThan(int axis, double threshold,
                                         EventLoop* loop) const {
  return BooleanEvent(loop, [this, axis, threshold]() {
    return this->GetRawAxis(axis) > threshold;
  });
}

int GenericHID::GetAxesMaximumIndex() const {
  return DriverStation::GetStickAxesMaximumIndex(m_port);
}

int GenericHID::GetAxesAvailable() const {
  return DriverStation::GetStickAxesAvailable(m_port);
}

int GenericHID::GetPOVsMaximumIndex() const {
  return DriverStation::GetStickPOVsMaximumIndex(m_port);
}

int GenericHID::GetPOVsAvailable() const {
  return DriverStation::GetStickPOVsAvailable(m_port);
}

int GenericHID::GetButtonsMaximumIndex() const {
  return DriverStation::GetStickButtonsMaximumIndex(m_port);
}

uint64_t GenericHID::GetButtonsAvailable() const {
  return DriverStation::GetStickButtonsAvailable(m_port);
}

bool GenericHID::IsConnected() const {
  return DriverStation::IsJoystickConnected(m_port);
}

GenericHID::HIDType GenericHID::GetGamepadType() const {
  return static_cast<HIDType>(DriverStation::GetJoystickGamepadType(m_port));
}

GenericHID::SupportedOutputs GenericHID::GetSupportedOutputs() const {
  return static_cast<SupportedOutputs>(
      DriverStation::GetJoystickSupportedOutputs(m_port));
}

std::string GenericHID::GetName() const {
  return DriverStation::GetJoystickName(m_port);
}

int GenericHID::GetPort() const {
  return m_port;
}

void GenericHID::SetLeds(int r, int g, int b) {
  uint32_t value = (static_cast<uint32_t>(r & 0xFF) << 16) |
                   (static_cast<uint32_t>(g & 0xFF) << 8) |
                   static_cast<uint32_t>(b & 0xFF);
  HAL_SetJoystickLeds(m_port, value);
}

void GenericHID::SetRumble(RumbleType type, double value) {
  value = std::clamp(value, 0.0, 1.0);
  double rumbleValue = value * 65535;

  if (type == kLeftRumble) {
    m_leftRumble = rumbleValue;
  } else if (type == kRightRumble) {
    m_rightRumble = rumbleValue;
  } else if (type == kLeftTriggerRumble) {
    m_leftTriggerRumble = rumbleValue;
  } else if (type == kRightTriggerRumble) {
    m_rightTriggerRumble = rumbleValue;
  }

  HAL_SetJoystickRumble(m_port, m_leftRumble, m_rightRumble,
                        m_leftTriggerRumble, m_rightTriggerRumble);
}
