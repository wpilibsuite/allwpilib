/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
