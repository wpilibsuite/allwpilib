/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
