// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/GenericHIDSim.hpp"

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/driverstation/GenericHID.hpp"
#include "wpi/simulation/DriverStationSim.hpp"

using namespace wpi;
using namespace wpi::sim;

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

void GenericHIDSim::SetPOV(int pov, DriverStation::POVDirection value) {
  DriverStationSim::SetJoystickPOV(m_port, pov, value);
}

void GenericHIDSim::SetPOV(DriverStation::POVDirection value) {
  SetPOV(0, value);
}

void GenericHIDSim::SetAxesMaximumIndex(int maximumIndex) {
  DriverStationSim::SetJoystickAxesMaximumIndex(m_port, maximumIndex);
}

void GenericHIDSim::SetAxesAvailable(int count) {
  DriverStationSim::SetJoystickAxesAvailable(m_port, count);
}

void GenericHIDSim::SetPOVsMaximumIndex(int maximumIndex) {
  DriverStationSim::SetJoystickPOVsMaximumIndex(m_port, maximumIndex);
}

void GenericHIDSim::SetPOVsAvailable(int count) {
  DriverStationSim::SetJoystickPOVsAvailable(m_port, count);
}

void GenericHIDSim::SetButtonsMaximumIndex(int maximumIndex) {
  DriverStationSim::SetJoystickButtonsMaximumIndex(m_port, maximumIndex);
}

void GenericHIDSim::SetButtonsAvailable(uint64_t count) {
  DriverStationSim::SetJoystickButtonsAvailable(m_port, count);
}

void GenericHIDSim::SetGamepadType(GenericHID::HIDType type) {
  DriverStationSim::SetJoystickGamepadType(m_port, type);
}

void GenericHIDSim::SetSupportedOutputs(
    GenericHID::SupportedOutputs supportedOutputs) {
  DriverStationSim::SetJoystickSupportedOutputs(m_port, supportedOutputs);
}

void GenericHIDSim::SetName(const char* name) {
  DriverStationSim::SetJoystickName(m_port, name);
}

int32_t GenericHIDSim::GetLeds() {
  return DriverStationSim::GetJoystickLeds(m_port);
}

double GenericHIDSim::GetRumble(GenericHID::RumbleType type) {
  int intType = 0;
  switch (type) {
    case GenericHID::kLeftRumble:
      intType = 0;
      break;
    case GenericHID::kRightRumble:
      intType = 1;
      break;
    case GenericHID::kLeftTriggerRumble:
      intType = 2;
      break;
    case GenericHID::kRightTriggerRumble:
      intType = 3;
      break;
    default:
      return 0.0;
  }
  int value = DriverStationSim::GetJoystickRumble(m_port, intType);
  return value / 65535.0;
}
