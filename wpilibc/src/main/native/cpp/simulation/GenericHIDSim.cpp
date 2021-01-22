// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/GenericHIDSim.h"

#include "frc/GenericHID.h"
#include "frc/simulation/DriverStationSim.h"

using namespace frc;
using namespace frc::sim;

GenericHIDSim::GenericHIDSim(const GenericHID& joystick)
    : m_port{joystick.GetPort()} {}

GenericHIDSim::GenericHIDSim(int port) : m_port{port} {}

void GenericHIDSim::NotifyNewData() {
  DriverStationSim::NotifyNewData();
}

void GenericHIDSim::SetRawButton(int button, bool value) {
  DriverStationSim::SetJoystickButton(m_port, button, value);
}

void GenericHIDSim::SetRawAxis(int axis, double value) {
  DriverStationSim::SetJoystickAxis(m_port, axis, value);
}

void GenericHIDSim::SetPOV(int pov, int value) {
  DriverStationSim::SetJoystickPOV(m_port, pov, value);
}

void GenericHIDSim::SetPOV(int value) {
  SetPOV(0, value);
}

void GenericHIDSim::SetAxisCount(int count) {
  DriverStationSim::SetJoystickAxisCount(m_port, count);
}

void GenericHIDSim::SetPOVCount(int count) {
  DriverStationSim::SetJoystickPOVCount(m_port, count);
}

void GenericHIDSim::SetButtonCount(int count) {
  DriverStationSim::SetJoystickButtonCount(m_port, count);
}

void GenericHIDSim::SetType(GenericHID::HIDType type) {
  DriverStationSim::SetJoystickType(m_port, type);
}

void GenericHIDSim::SetName(const char* name) {
  DriverStationSim::SetJoystickName(m_port, name);
}

void GenericHIDSim::SetAxisType(int axis, int type) {
  DriverStationSim::SetJoystickAxisType(m_port, axis, type);
}

bool GenericHIDSim::GetOutput(int outputNumber) {
  int64_t outputs = GetOutputs();
  return (outputs & (static_cast<int64_t>(1) << (outputNumber - 1))) != 0;
}

int64_t GenericHIDSim::GetOutputs() {
  return DriverStationSim::GetJoystickOutputs(m_port);
}

double GenericHIDSim::GetRumble(GenericHID::RumbleType type) {
  int value = DriverStationSim::GetJoystickRumble(
      m_port, type == GenericHID::kLeftRumble ? 0 : 1);
  return value / 65535.0;
}
