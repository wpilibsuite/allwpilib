// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "frc/GenericHID.h"

namespace frc {

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
  void SetPOV(int pov, int value);

  /**
   * Set the value of the default POV (port 0).
   *
   * @param value the new value
   */
  void SetPOV(int value);

  /**
   * Set the axis count of this device.
   *
   * @param count the new axis count
   */
  void SetAxisCount(int count);

  /**
   * Set the POV count of this device.
   *
   * @param count the new POV count
   */
  void SetPOVCount(int count);

  /**
   * Set the button count of this device.
   *
   * @param count the new button count
   */
  void SetButtonCount(int count);

  /**
   * Set the type of this device.
   *
   * @param type the new device type
   */
  void SetType(GenericHID::HIDType type);

  /**
   * Set the name of this device.
   *
   * @param name the new device name
   */
  void SetName(const char* name);

  /**
   * Set the type of an axis.
   *
   * @param axis the axis
   * @param type the type
   */
  void SetAxisType(int axis, int type);

  /**
   * Read the output of a button.
   *
   * @param outputNumber the button number
   * @return the value of the button (true = pressed)
   */
  bool GetOutput(int outputNumber);

  /**
   * Get the encoded 16-bit integer that passes button values.
   *
   * @return the button values
   */
  int64_t GetOutputs();

  /**
   * Get the joystick rumble.
   *
   * @param type the rumble to read
   * @return the rumble value
   */
  double GetRumble(GenericHID::RumbleType type);

 protected:
  int m_port;
};

}  // namespace sim
}  // namespace frc
