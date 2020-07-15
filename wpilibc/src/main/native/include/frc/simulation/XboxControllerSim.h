/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/simulation/GenericHIDSim.h"

namespace frc {

class XboxController;

namespace sim {

/**
 * Class to control a simulated Xbox 360 or Xbox One controller.
 */
class XboxControllerSim : public GenericHIDSim {
 public:
  /**
   * Constructs from a XboxController object.
   *
   * @param joystick controller to simulate
   */
  explicit XboxControllerSim(const XboxController& joystick);

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  explicit XboxControllerSim(int port);

  void SetX(GenericHID::JoystickHand hand, double value);

  void SetY(GenericHID::JoystickHand hand, double value);

  void SetTriggerAxis(GenericHID::JoystickHand hand, double value);

  void SetBumper(GenericHID::JoystickHand hand, bool state);

  void SetStickButton(GenericHID::JoystickHand hand, bool state);

  void SetAButton(bool state);

  void SetBButton(bool state);

  void SetXButton(bool state);

  void SetYButton(bool state);

  void SetBackButton(bool state);

  void SetStartButton(bool state);
};

}  // namespace sim
}  // namespace frc
