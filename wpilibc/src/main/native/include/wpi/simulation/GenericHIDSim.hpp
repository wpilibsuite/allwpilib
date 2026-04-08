// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/driverstation/GenericHID.hpp"

namespace wpi {

class GenericHID;

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
  explicit GenericHIDSim(const GenericHID& joystick);

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  explicit GenericHIDSim(int port);

  /**
   * Updates joystick data so that new values are visible to the user program.
   */
  void NotifyNewData();

  /**
   * Set the value of a given button.
   *
   * @param button the button to set
   * @param value the new value
   */
  void SetRawButton(int button, bool value);

  /**
   * Set the value of a given axis.
   *
   * @param axis the axis to set
   * @param value the new value
   */
  void SetRawAxis(int axis, double value);

  /**
   * Set the value of a given POV.
   *
   * @param pov the POV to set
   * @param value the new value
   */
  void SetPOV(int pov, DriverStation::POVDirection value);

  /**
   * Set the value of the default POV (port 0).
   *
   * @param value the new value
   */
  void SetPOV(DriverStation::POVDirection value);

  void SetAxesMaximumIndex(int maximumIndex);

  /**
   * Set the axis count of this device.
   *
   * @param count the new axis count
   */
  void SetAxesAvailable(int count);

  void SetPOVsMaximumIndex(int maximumIndex);

  /**
   * Set the POV count of this device.
   *
   * @param count the new POV count
   */
  void SetPOVsAvailable(int count);

  void SetButtonsMaximumIndex(int maximumIndex);

  /**
   * Set the button count of this device.
   *
   * @param count the new button count
   */
  void SetButtonsAvailable(uint64_t count);

  /**
   * Set the type of this device.
   *
   * @param type the new device type
   */
  void SetGamepadType(GenericHID::HIDType type);

  void SetSupportedOutputs(GenericHID::SupportedOutputs supportedOutputs);

  /**
   * Set the name of this device.
   *
   * @param name the new device name
   */
  void SetName(const char* name);

  /**
   * Get the value of set LEDs.
   *
   * @return the led color
   */
  int32_t GetLeds();

  /**
   * Get the joystick rumble.
   *
   * @param type the rumble to read
   * @return the rumble value
   */
  double GetRumble(GenericHID::RumbleType type);

 protected:
  /// GenericHID port.
  int m_port;
};

}  // namespace sim
}  // namespace wpi
