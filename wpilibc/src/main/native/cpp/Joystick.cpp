/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Joystick.h"

#include <cmath>

#include <hal/FRCUsageReporting.h>
#include <wpi/math>

using namespace frc;

Joystick::Joystick(int port) : GenericHID(port) {
  m_axes[Axis::kX] = kDefaultXChannel;
  m_axes[Axis::kY] = kDefaultYChannel;
  m_axes[Axis::kZ] = kDefaultZChannel;
  m_axes[Axis::kTwist] = kDefaultTwistChannel;
  m_axes[Axis::kThrottle] = kDefaultThrottleChannel;

  HAL_Report(HALUsageReporting::kResourceType_Joystick, port + 1);
}

void Joystick::SetXChannel(int channel) { m_axes[Axis::kX] = channel; }

void Joystick::SetYChannel(int channel) { m_axes[Axis::kY] = channel; }

void Joystick::SetZChannel(int channel) { m_axes[Axis::kZ] = channel; }

void Joystick::SetTwistChannel(int channel) { m_axes[Axis::kTwist] = channel; }

void Joystick::SetThrottleChannel(int channel) {
  m_axes[Axis::kThrottle] = channel;
}

int Joystick::GetXChannel() const { return m_axes[Axis::kX]; }

int Joystick::GetYChannel() const { return m_axes[Axis::kY]; }

int Joystick::GetZChannel() const { return m_axes[Axis::kZ]; }

int Joystick::GetTwistChannel() const { return m_axes[Axis::kTwist]; }

int Joystick::GetThrottleChannel() const { return m_axes[Axis::kThrottle]; }

double Joystick::GetX(JoystickHand hand) const {
  return GetRawAxis(m_axes[Axis::kX]);
}

double Joystick::GetY(JoystickHand hand) const {
  return GetRawAxis(m_axes[Axis::kY]);
}

double Joystick::GetZ() const { return GetRawAxis(m_axes[Axis::kZ]); }

double Joystick::GetTwist() const { return GetRawAxis(m_axes[Axis::kTwist]); }

double Joystick::GetThrottle() const {
  return GetRawAxis(m_axes[Axis::kThrottle]);
}

bool Joystick::GetTrigger() const { return GetRawButton(Button::kTrigger); }

bool Joystick::GetTriggerPressed() {
  return GetRawButtonPressed(Button::kTrigger);
}

bool Joystick::GetTriggerReleased() {
  return GetRawButtonReleased(Button::kTrigger);
}

bool Joystick::GetTop() const { return GetRawButton(Button::kTop); }

bool Joystick::GetTopPressed() { return GetRawButtonPressed(Button::kTop); }

bool Joystick::GetTopReleased() { return GetRawButtonReleased(Button::kTop); }

double Joystick::GetMagnitude() const {
  return std::sqrt(std::pow(GetX(), 2) + std::pow(GetY(), 2));
}

double Joystick::GetDirectionRadians() const {
  return std::atan2(GetX(), -GetY());
}

double Joystick::GetDirectionDegrees() const {
  return (180 / wpi::math::pi) * GetDirectionRadians();
}
