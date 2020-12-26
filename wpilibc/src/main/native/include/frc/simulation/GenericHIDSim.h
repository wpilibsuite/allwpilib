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

  void SetRawButton(int button, bool value);

  void SetRawAxis(int axis, double value);

  void SetPOV(int pov, int value);

  void SetPOV(int value);

  void SetAxisCount(int count);

  void SetPOVCount(int count);

  void SetButtonCount(int count);

  void SetType(GenericHID::HIDType type);

  void SetName(const char* name);

  void SetAxisType(int axis, int type);

  bool GetOutput(int outputNumber);

  int64_t GetOutputs();

  double GetRumble(GenericHID::RumbleType type);

 protected:
  int m_port;
};

}  // namespace sim
}  // namespace frc
