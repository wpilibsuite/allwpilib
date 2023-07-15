// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/simulation/GenericHIDSim.h"

namespace frc {

class PS4Controller;

namespace sim {

/**
 * Class to control a simulated PS4 controller.
 */
class PS4ControllerSim : public GenericHIDSim {
 public:
  /**
   * Constructs from a PS4Controller object.
   *
   * @param joystick controller to simulate
   */
  explicit PS4ControllerSim(const PS4Controller& joystick);

  /**
   * Constructs from a joystick port number.
   *
   * @param port port number
   */
  explicit PS4ControllerSim(int port);

  /**
   * Change the X axis value of the controller's left stick.
   *
   * @param value the new value
   */
  void SetLeftX(double value);

  /**
   * Change the X axis value of the controller's right stick.
   *
   * @param value the new value
   */
  void SetRightX(double value);

  /**
   * Change the Y axis value of the controller's left stick.
   *
   * @param value the new value
   */
  void SetLeftY(double value);

  /**
   * Change the Y axis value of the controller's right stick.
   *
   * @param value the new value
   */
  void SetRightY(double value);

  /**
   * Change the L2 axis axis value of the controller.
   *
   * @param value the new value
   */
  void SetL2Axis(double value);

  /**
   * Change the R2 axis value of the controller.
   *
   * @param value the new value
   */
  void SetR2Axis(double value);

  /**
   * Change the value of the Square button on the controller.
   *
   * @param value the new value
   */
  void SetSquareButton(bool value);

  /**
   * Change the value of the Cross button on the controller.
   *
   * @param value the new value
   */
  void SetCrossButton(bool value);

  /**
   * Change the value of the Circle button on the controller.
   *
   * @param value the new value
   */
  void SetCircleButton(bool value);

  /**
   * Change the value of the Triangle button on the controller.
   *
   * @param value the new value
   */
  void SetTriangleButton(bool value);

  /**
   * Change the value of the L1 button on the controller.
   *
   * @param value the new value
   */
  void SetL1Button(bool value);

  /**
   * Change the value of the R1 button on the controller.
   *
   * @param value the new value
   */
  void SetR1Button(bool value);

  /**
   * Change the value of the L2 button on the controller.
   *
   * @param value the new value
   */
  void SetL2Button(bool value);

  /**
   * Change the value of the R2 button on the controller.
   *
   * @param value the new value
   */
  void SetR2Button(bool value);

  /**
   * Change the value of the Share button on the controller.
   *
   * @param value the new value
   */
  void SetShareButton(bool value);

  /**
   * Change the value of the Options button on the controller.
   *
   * @param value the new value
   */
  void SetOptionsButton(bool value);

  /**
   * Change the value of the L3 (left stick) button on the controller.
   *
   * @param value the new value
   */
  void SetL3Button(bool value);

  /**
   * Change the value of the R3 (right stick) button on the controller.
   *
   * @param value the new value
   */
  void SetR3Button(bool value);

  /**
   * Change the value of the PS button on the controller.
   *
   * @param value the new value
   */
  void SetPSButton(bool value);

  /**
   * Change the value of the touchpad button on the controller.
   *
   * @param value the new value
   */
  void SetTouchpad(bool value);
};

}  // namespace sim
}  // namespace frc
