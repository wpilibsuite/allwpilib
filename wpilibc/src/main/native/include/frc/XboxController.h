/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/GenericHID.h"

namespace frc {

/**
 * Handle input from Xbox 360 or Xbox One controllers connected to the Driver
 * Station.
 *
 * This class handles Xbox input that comes from the Driver Station. Each time a
 * value is requested the most recent value is returned. There is a single class
 * instance for each controller and the mapping of ports to hardware buttons
 * depends on the code in the Driver Station.
 */
class XboxController : public GenericHID {
 public:
  /**
   * Construct an instance of an Xbox controller.
   *
   * The controller index is the USB port on the Driver Station.
   *
   * @param port The port on the Driver Station that the controller is plugged
   *             into (0-5).
   */
  explicit XboxController(int port);

  virtual ~XboxController() = default;

  XboxController(XboxController&&) = default;
  XboxController& operator=(XboxController&&) = default;

  /**
   * Get the X axis value of the controller.
   *
   * @param hand Side of controller whose value should be returned.
   */
  double GetX(JoystickHand hand) const override;

  /**
   * Get the Y axis value of the controller.
   *
   * @param hand Side of controller whose value should be returned.
   */
  double GetY(JoystickHand hand) const override;

  /**
   * Get the trigger axis value of the controller.
   *
   * @param hand Side of controller whose value should be returned.
   */
  double GetTriggerAxis(JoystickHand hand) const;

  /**
   * Read the value of the bumper button on the controller.
   *
   * @param hand Side of controller whose value should be returned.
   */
  bool GetBumper(JoystickHand hand) const;

  /**
   * Whether the bumper was pressed since the last check.
   *
   * @param hand Side of controller whose value should be returned.
   * @return Whether the button was pressed since the last check.
   */
  bool GetBumperPressed(JoystickHand hand);

  /**
   * Whether the bumper was released since the last check.
   *
   * @param hand Side of controller whose value should be returned.
   * @return Whether the button was released since the last check.
   */
  bool GetBumperReleased(JoystickHand hand);

  /**
   * Read the value of the stick button on the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The state of the button.
   */
  bool GetStickButton(JoystickHand hand) const;

  /**
   * Whether the stick button was pressed since the last check.
   *
   * @param hand Side of controller whose value should be returned.
   * @return Whether the button was pressed since the last check.
   */
  bool GetStickButtonPressed(JoystickHand hand);

  /**
   * Whether the stick button was released since the last check.
   *
   * @param hand Side of controller whose value should be returned.
   * @return Whether the button was released since the last check.
   */
  bool GetStickButtonReleased(JoystickHand hand);

  /**
   * Read the value of the A button on the controller.
   *
   * @return The state of the button.
   */
  bool GetAButton() const;

  /**
   * Whether the A button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetAButtonPressed();

  /**
   * Whether the A button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetAButtonReleased();

  /**
   * Read the value of the B button on the controller.
   *
   * @return The state of the button.
   */
  bool GetBButton() const;

  /**
   * Whether the B button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetBButtonPressed();

  /**
   * Whether the B button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetBButtonReleased();

  /**
   * Read the value of the X button on the controller.
   *
   * @return The state of the button.
   */
  bool GetXButton() const;

  /**
   * Whether the X button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetXButtonPressed();

  /**
   * Whether the X button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetXButtonReleased();

  /**
   * Read the value of the Y button on the controller.
   *
   * @return The state of the button.
   */
  bool GetYButton() const;

  /**
   * Whether the Y button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetYButtonPressed();

  /**
   * Whether the Y button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetYButtonReleased();

  /**
   * Whether the Y button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetBackButton() const;

  /**
   * Whether the back button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetBackButtonPressed();

  /**
   * Whether the back button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetBackButtonReleased();

  /**
   * Read the value of the start button on the controller.
   *
   * @param hand Side of controller whose value should be returned.
   * @return The state of the button.
   */
  bool GetStartButton() const;

  /**
   * Whether the start button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetStartButtonPressed();

  /**
   * Whether the start button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetStartButtonReleased();

  enum class Button {
    kBumperLeft = 5,
    kBumperRight = 6,
    kStickLeft = 9,
    kStickRight = 10,
    kA = 1,
    kB = 2,
    kX = 3,
    kY = 4,
    kBack = 7,
    kStart = 8
  };

  enum class Axis {
    kLeftX = 0,
    kRightX = 4,
    kLeftY = 1,
    kRightY = 5,
    kLeftTrigger = 2,
    kRightTrigger = 3
  };
};

}  // namespace frc
