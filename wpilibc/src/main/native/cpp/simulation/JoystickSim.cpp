// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/JoystickSim.h"

#include "frc/Joystick.h"
#include "frc/simulation/GenericHIDSim.h"

using namespace frc;
using namespace frc::sim;

JoystickSim::JoystickSim(const Joystick& joystick)
    : GenericHIDSim{joystick}, m_joystick{&joystick} {
  // default to a reasonable joystick configuration
  SetAxisCount(5);
  SetButtonCount(12);
  SetPOVCount(1);
}

JoystickSim::JoystickSim(int port) : GenericHIDSim{port} {
  // default to a reasonable joystick configuration
  SetAxisCount(5);
  SetButtonCount(12);
  SetPOVCount(1);
}

void JoystickSim::SetX(double value) {
  SetRawAxis(
      m_joystick ? m_joystick->GetXChannel() : Joystick::DEFAULT_X_CHANNEL,
      value);
}

void JoystickSim::SetY(double value) {
  SetRawAxis(
      m_joystick ? m_joystick->GetYChannel() : Joystick::DEFAULT_Y_CHANNEL,
      value);
}

void JoystickSim::SetZ(double value) {
  SetRawAxis(
      m_joystick ? m_joystick->GetZChannel() : Joystick::DEFAULT_Z_CHANNEL,
      value);
}

void JoystickSim::SetTwist(double value) {
  SetRawAxis(m_joystick ? m_joystick->GetTwistChannel()
                        : Joystick::DEFAULT_TWIST_CHANNEL,
             value);
}

void JoystickSim::SetThrottle(double value) {
  SetRawAxis(m_joystick ? m_joystick->GetThrottleChannel()
                        : Joystick::DEFAULT_THROTTLE_CHANNEL,
             value);
}

void JoystickSim::SetTrigger(bool state) {
  SetRawButton(1, state);
}

void JoystickSim::SetTop(bool state) {
  SetRawButton(2, state);
}
