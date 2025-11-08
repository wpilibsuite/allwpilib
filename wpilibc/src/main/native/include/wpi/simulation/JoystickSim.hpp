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

  /**
   * Set the X value of the joystick.
   *
   * @param value the new X value
   */
  void SetX(double value);

  /**
   * Set the Y value of the joystick.
   *
   * @param value the new Y value
   */
  void SetY(double value);

  /**
   * Set the Z value of the joystick.
   *
   * @param value the new Z value
   */
  void SetZ(double value);

  /**
   * Set the twist value of the joystick.
   *
   * @param value the new twist value
   */
  void SetTwist(double value);

  /**
   * Set the throttle value of the joystick.
   *
   * @param value the new throttle value
   */
  void SetThrottle(double value);

  /**
   * Set the trigger value of the joystick.
   *
   * @param state the new value
   */
  void SetTrigger(bool state);

  /**
   * Set the top state of the joystick.
   *
   * @param state the new state
   */
  void SetTop(bool state);

 private:
  const Joystick* m_joystick = nullptr;
};

}  // namespace sim
}  // namespace frc
