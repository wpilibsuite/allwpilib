/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/GenericHID.h"
#include "frc/simulation/DriverStationSim.h"

namespace frc {
namespace sim {

/**
 * Class to control a simulated generic joystick.
 */
class GenericHIDSim {
 public:
  /**
   * Constructs from a GenericHID object.
   *
   * @param joystick joystick to simulate
   */
  explicit GenericHIDSim(const GenericHID& joystick)
      : m_port{joystick.GetPort()} {}

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  explicit GenericHIDSim(int port) : m_port{port} {}

  /**
   * Updates joystick data so that new values are visible to the user program.
   */
  void NotifyNewData() { DriverStationSim::NotifyNewData(); }

  void SetRawButton(int button, bool value) {
    DriverStationSim::SetJoystickButton(m_port, button, value);
  }

  void SetRawAxis(int axis, double value) {
    DriverStationSim::SetJoystickAxis(m_port, axis, value);
  }

  void SetPOV(int pov, int value) {
    DriverStationSim::SetJoystickPOV(m_port, pov, value);
  }

  void SetPOV(int value) { SetPOV(0, value); }

  void SetAxisCount(int count) {
    DriverStationSim::SetJoystickAxisCount(m_port, count);
  }

  void SetPOVCount(int count) {
    DriverStationSim::SetJoystickPOVCount(m_port, count);
  }

  void SetButtonCount(int count) {
    DriverStationSim::SetJoystickButtonCount(m_port, count);
  }

  void SetType(GenericHID::HIDType type) {
    DriverStationSim::SetJoystickType(m_port, type);
  }

  void SetName(const char* name) {
    DriverStationSim::SetJoystickName(m_port, name);
  }

  void SetAxisType(int axis, int type) {
    DriverStationSim::SetJoystickAxisType(m_port, axis, type);
  }

  bool GetOutput(int outputNumber) {
    int64_t outputs = GetOutputs();
    return (outputs & (static_cast<int64_t>(1) << (outputNumber - 1))) != 0;
  }

  int64_t GetOutputs() { return DriverStationSim::GetJoystickOutputs(m_port); }

  double GetRumble(GenericHID::RumbleType type) {
    int value = DriverStationSim::GetJoystickRumble(
        m_port, type == GenericHID::kLeftRumble ? 0 : 1);
    return value / 65535.0;
  }

 protected:
  int m_port;
};

}  // namespace sim
}  // namespace frc
