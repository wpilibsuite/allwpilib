// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

  /**
   * Change the X value of the joystick.
   *
   * @param hand the joystick hand
   * @param value the new value
   */
  void SetX(GenericHID::JoystickHand hand, double value);

  /**
   * Change the Y value of the joystick.
   *
   * @param hand the joystick hand
   * @param value the new value
   */
  void SetY(GenericHID::JoystickHand hand, double value);

  /**
   * Change the value of a trigger axis on the joystick.
   *
   * @param hand the joystick hand
   * @param value the new value
   */
  void SetTriggerAxis(GenericHID::JoystickHand hand, double value);

  /**
   * Change the value of a bumper on the joystick.
   *
   * @param hand the joystick hand
   * @param state the new value
   */
  void SetBumper(GenericHID::JoystickHand hand, bool state);

  /**
   * Change the value of a button on the joystick.
   *
   * @param hand the joystick hand
   * @param state the new value
   */
  void SetStickButton(GenericHID::JoystickHand hand, bool state);

  /**
   * Change the value of the A button.
   *
   * @param state the new value
   */
  void SetAButton(bool state);

  /**
   * Change the value of the B button.
   *
   * @param state the new value
   */
  void SetBButton(bool state);

  /**
   * Change the value of the X button.
   *
   * @param state the new value
   */
  void SetXButton(bool state);

  /**
   * Change the value of the Y button.
   *
   * @param state the new value
   */
  void SetYButton(bool state);

  /**
   * Change the value of the Back button.
   *
   * @param state the new value
   */
  void SetBackButton(bool state);

  /**
   * Change the value of the Start button.
   *
   * @param state the new value
   */
  void SetStartButton(bool state);
};

}  // namespace sim
}  // namespace frc
