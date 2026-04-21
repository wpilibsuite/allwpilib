// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/GenericHID.hpp"

#include <string>

#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/event/BooleanEvent.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/system/Errors.hpp"

using namespace wpi;

GenericHID::GenericHID(int port) {
  if (port < 0 || port >= wpi::internal::DriverStationBackend::JOYSTICK_PORTS) {
    throw WPILIB_MakeError(warn::BadJoystickIndex, "port {} out of range",
                           port);
  }
  m_port = port;
}

bool GenericHID::GetRawButton(int button) const {
  return wpi::internal::DriverStationBackend::GetStickButton(m_port, button);
}

bool GenericHID::GetRawButtonPressed(int button) {
  return wpi::internal::DriverStationBackend::GetStickButtonPressed(m_port,
                                                                    button);
}

bool GenericHID::GetRawButtonReleased(int button) {
  return wpi::internal::DriverStationBackend::GetStickButtonReleased(m_port,
                                                                     button);
}

BooleanEvent GenericHID::Button(int button, EventLoop* loop) const {
  return BooleanEvent(loop,
                      [this, button]() { return this->GetRawButton(button); });
}

double GenericHID::GetRawAxis(int axis) const {
  return wpi::internal::DriverStationBackend::GetStickAxis(m_port, axis);
}

POVDirection GenericHID::GetPOV(int pov) const {
  return wpi::internal::DriverStationBackend::GetStickPOV(m_port, pov);
}

BooleanEvent GenericHID::POV(POVDirection angle, EventLoop* loop) const {
  return POV(0, angle, loop);
}

BooleanEvent GenericHID::POV(int pov, POVDirection angle,
                             EventLoop* loop) const {
  return BooleanEvent(
      loop, [this, pov, angle] { return this->GetPOV(pov) == angle; });
}

BooleanEvent GenericHID::POVUp(EventLoop* loop) const {
  return POV(POVDirection::UP, loop);
}

BooleanEvent GenericHID::POVUpRight(EventLoop* loop) const {
  return POV(POVDirection::UP_RIGHT, loop);
}

BooleanEvent GenericHID::POVRight(EventLoop* loop) const {
  return POV(POVDirection::RIGHT, loop);
}

BooleanEvent GenericHID::POVDownRight(EventLoop* loop) const {
  return POV(POVDirection::DOWN_RIGHT, loop);
}

BooleanEvent GenericHID::POVDown(EventLoop* loop) const {
  return POV(POVDirection::DOWN, loop);
}

BooleanEvent GenericHID::POVDownLeft(EventLoop* loop) const {
  return POV(POVDirection::DOWN_LEFT, loop);
}

BooleanEvent GenericHID::POVLeft(EventLoop* loop) const {
  return POV(POVDirection::LEFT, loop);
}

BooleanEvent GenericHID::POVUpLeft(EventLoop* loop) const {
  return POV(POVDirection::UP_LEFT, loop);
}

BooleanEvent GenericHID::POVCenter(EventLoop* loop) const {
  return POV(POVDirection::CENTER, loop);
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
  return wpi::internal::DriverStationBackend::GetStickAxesMaximumIndex(m_port);
}

int GenericHID::GetAxesAvailable() const {
  return wpi::internal::DriverStationBackend::GetStickAxesAvailable(m_port);
}

int GenericHID::GetPOVsMaximumIndex() const {
  return wpi::internal::DriverStationBackend::GetStickPOVsMaximumIndex(m_port);
}

int GenericHID::GetPOVsAvailable() const {
  return wpi::internal::DriverStationBackend::GetStickPOVsAvailable(m_port);
}

int GenericHID::GetButtonsMaximumIndex() const {
  return wpi::internal::DriverStationBackend::GetStickButtonsMaximumIndex(
      m_port);
}

uint64_t GenericHID::GetButtonsAvailable() const {
  return wpi::internal::DriverStationBackend::GetStickButtonsAvailable(m_port);
}

bool GenericHID::IsConnected() const {
  return wpi::internal::DriverStationBackend::IsJoystickConnected(m_port);
}

GenericHID::HIDType GenericHID::GetGamepadType() const {
  return static_cast<HIDType>(
      wpi::internal::DriverStationBackend::GetJoystickGamepadType(m_port));
}

GenericHID::SupportedOutputs GenericHID::GetSupportedOutputs() const {
  return static_cast<SupportedOutputs>(
      wpi::internal::DriverStationBackend::GetJoystickSupportedOutputs(m_port));
}

std::string GenericHID::GetName() const {
  return wpi::internal::DriverStationBackend::GetJoystickName(m_port);
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

  if (type == RumbleType::LEFT_RUMBLE) {
    m_leftRumble = rumbleValue;
  } else if (type == RumbleType::RIGHT_RUMBLE) {
    m_rightRumble = rumbleValue;
  } else if (type == RumbleType::LEFT_TRIGGER_RUMBLE) {
    m_leftTriggerRumble = rumbleValue;
  } else if (type == RumbleType::RIGHT_TRIGGER_RUMBLE) {
    m_rightTriggerRumble = rumbleValue;
  }

  HAL_SetJoystickRumble(m_port, m_leftRumble, m_rightRumble,
                        m_leftTriggerRumble, m_rightTriggerRumble);
}

bool GenericHID::GetTouchpadFingerAvailable(int touchpad, int finger) const {
  return wpi::internal::DriverStationBackend::GetStickTouchpadFingerAvailable(
      m_port, touchpad, finger);
}

TouchpadFinger GenericHID::GetTouchpadFinger(int touchpad, int finger) const {
  return wpi::internal::DriverStationBackend::GetStickTouchpadFinger(
      m_port, touchpad, finger);
}
