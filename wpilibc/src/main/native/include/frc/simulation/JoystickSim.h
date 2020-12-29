// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/simulation/GenericHIDSim.h"

namespace frc {

class Joystick;

namespace sim {

/**
 * Class to control a simulated joystick.
 */
class JoystickSim : public GenericHIDSim {
 public:
  /**
   * Constructs from a Joystick object.
   *
   * @param joystick joystick to simulate
   */
  explicit JoystickSim(const Joystick& joystick);

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  explicit JoystickSim(int port);

  void SetX(double value);

  void SetY(double value);

  void SetZ(double value);

  void SetTwist(double value);

  void SetThrottle(double value);

  void SetTrigger(bool state);

  void SetTop(bool state);

 private:
  const Joystick* m_joystick = nullptr;
};

}  // namespace sim
}  // namespace frc
