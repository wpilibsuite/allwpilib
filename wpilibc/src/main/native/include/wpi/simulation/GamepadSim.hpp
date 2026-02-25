// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/simulation/GenericHIDSim.hpp"

namespace wpi {

class Gamepad;

namespace sim {

/**
 * Class to control a simulated Gamepad controller.
 */
class GamepadSim : public GenericHIDSim {
 public:
  /**
   * Constructs from a Gamepad object.
   *
   * @param joystick controller to simulate
   */
  explicit GamepadSim(const Gamepad& joystick);

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  explicit GamepadSim(int port);

  /**
   * Change the left X value of the controller's joystick.
   *
   * @param value the new value
   */
  void SetLeftX(double value);

  /**
   * Change the left Y value of the controller's joystick.
   *
   * @param value the new value
   */
  void SetLeftY(double value);

  /**
   * Change the right X value of the controller's joystick.
   *
   * @param value the new value
   */
  void SetRightX(double value);

  /**
   * Change the right Y value of the controller's joystick.
   *
   * @param value the new value
   */
  void SetRightY(double value);

  /**
   * Change the value of the left trigger axis on the controller.
   *
   * @param value the new value
   */
  void SetLeftTriggerAxis(double value);

  /**
   * Change the value of the right trigger axis on the controller.
   *
   * @param value the new value
   */
  void SetRightTriggerAxis(double value);

  /**
   * Change the value of the South Face button on the controller.
   *
   * @param value the new value
   */
  void SetSouthFaceButton(bool value);

  /**
   * Change the value of the East Face button on the controller.
   *
   * @param value the new value
   */
  void SetEastFaceButton(bool value);

  /**
   * Change the value of the West Face button on the controller.
   *
   * @param value the new value
   */
  void SetWestFaceButton(bool value);

  /**
   * Change the value of the North Face button on the controller.
   *
   * @param value the new value
   */
  void SetNorthFaceButton(bool value);

  /**
   * Change the value of the Back button on the controller.
   *
   * @param value the new value
   */
  void SetBackButton(bool value);

  /**
   * Change the value of the Guide button on the controller.
   *
   * @param value the new value
   */
  void SetGuideButton(bool value);

  /**
   * Change the value of the Start button on the controller.
   *
   * @param value the new value
   */
  void SetStartButton(bool value);

  /**
   * Change the value of the left stick button on the controller.
   *
   * @param value the new value
   */
  void SetLeftStickButton(bool value);

  /**
   * Change the value of the right stick button on the controller.
   *
   * @param value the new value
   */
  void SetRightStickButton(bool value);

  /**
   * Change the value of the right bumper button on the controller.
   *
   * @param value the new value
   */
  void SetLeftBumperButton(bool value);

  /**
   * Change the value of the right bumper button on the controller.
   *
   * @param value the new value
   */
  void SetRightBumperButton(bool value);

  /**
   * Change the value of the D-pad up button on the controller.
   *
   * @param value the new value
   */
  void SetDpadUpButton(bool value);

  /**
   * Change the value of the D-pad down button on the controller.
   *
   * @param value the new value
   */
  void SetDpadDownButton(bool value);

  /**
   * Change the value of the D-pad left button on the controller.
   *
   * @param value the new value
   */
  void SetDpadLeftButton(bool value);

  /**
   * Change the value of the D-pad right button on the controller.
   *
   * @param value the new value
   */
  void SetDpadRightButton(bool value);

  /**
   * Change the value of the Miscellaneous 1 button on the controller.
   *
   * @param value the new value
   */
  void SetMisc1Button(bool value);

  /**
   * Change the value of the Right Paddle 1 button on the controller.
   *
   * @param value the new value
   */
  void SetRightPaddle1Button(bool value);

  /**
   * Change the value of the Left Paddle 1 button on the controller.
   *
   * @param value the new value
   */
  void SetLeftPaddle1Button(bool value);

  /**
   * Change the value of the Right Paddle 2 button on the controller.
   *
   * @param value the new value
   */
  void SetRightPaddle2Button(bool value);

  /**
   * Change the value of the Left Paddle 2 button on the controller.
   *
   * @param value the new value
   */
  void SetLeftPaddle2Button(bool value);

  /**
   * Change the value of the Touchpad button on the controller.
   *
   * @param value the new value
   */
  void SetTouchpadButton(bool value);

  /**
   * Change the value of the Miscellaneous 2 button on the controller.
   *
   * @param value the new value
   */
  void SetMisc2Button(bool value);

  /**
   * Change the value of the Miscellaneous 3 button on the controller.
   *
   * @param value the new value
   */
  void SetMisc3Button(bool value);

  /**
   * Change the value of the Miscellaneous 4 button on the controller.
   *
   * @param value the new value
   */
  void SetMisc4Button(bool value);

  /**
   * Change the value of the Miscellaneous 5 button on the controller.
   *
   * @param value the new value
   */
  void SetMisc5Button(bool value);

  /**
   * Change the value of the Miscellaneous 6 button on the controller.
   *
   * @param value the new value
   */
  void SetMisc6Button(bool value);
};

}  // namespace sim
}  // namespace wpi
