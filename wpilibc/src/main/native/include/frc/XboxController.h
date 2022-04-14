// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

  ~XboxController() override = default;

  XboxController(XboxController&&) = default;
  XboxController& operator=(XboxController&&) = default;

  /**
   * Get the X axis value of left side of the controller.
   */
  double GetLeftX() const;

  /**
   * Get the X axis value of right side of the controller.
   */
  double GetRightX() const;

  /**
   * Get the Y axis value of left side of the controller.
   */
  double GetLeftY() const;

  /**
   * Get the Y axis value of right side of the controller.
   */
  double GetRightY() const;

  /**
   * Get the left trigger (LT) axis value of the controller. Note that this axis
   * is bound to the range of [0, 1] as opposed to the usual [-1, 1].
   */
  double GetLeftTriggerAxis() const;

  /**
   * Get the right trigger (RT) axis value of the controller. Note that this
   * axis is bound to the range of [0, 1] as opposed to the usual [-1, 1].
   */
  double GetRightTriggerAxis() const;

  /**
   * Read the value of the left bumper (LB) button on the controller.
   */
  bool GetLeftBumper() const;

  /**
   * Read the value of the right bumper (RB) button on the controller.
   */
  bool GetRightBumper() const;

  /**
   * Whether the left bumper (LB) was pressed since the last check.
   */
  bool GetLeftBumperPressed();

  /**
   * Whether the right bumper (RB) was pressed since the last check.
   */
  bool GetRightBumperPressed();

  /**
   * Whether the left bumper (LB) was released since the last check.
   */
  bool GetLeftBumperReleased();

  /**
   * Whether the right bumper (RB) was released since the last check.
   */
  bool GetRightBumperReleased();

  /**
   * Read the value of the left stick button (LSB) on the controller.
   */
  bool GetLeftStickButton() const;

  /**
   * Read the value of the right stick button (RSB) on the controller.
   */
  bool GetRightStickButton() const;

  /**
   * Whether the left stick button (LSB) was pressed since the last check.
   */
  bool GetLeftStickButtonPressed();

  /**
   * Whether the right stick button (RSB) was pressed since the last check.
   */
  bool GetRightStickButtonPressed();

  /**
   * Whether the left stick button (LSB) was released since the last check.
   */
  bool GetLeftStickButtonReleased();

  /**
   * Whether the right stick button (RSB) was released since the last check.
   */
  bool GetRightStickButtonReleased();

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

  struct Button {
    static constexpr int kLeftBumper = 5;
    static constexpr int kRightBumper = 6;
    static constexpr int kLeftStick = 9;
    static constexpr int kRightStick = 10;
    static constexpr int kA = 1;
    static constexpr int kB = 2;
    static constexpr int kX = 3;
    static constexpr int kY = 4;
    static constexpr int kBack = 7;
    static constexpr int kStart = 8;
  };

  struct Axis {
    static constexpr int kLeftX = 0;
    static constexpr int kRightX = 4;
    static constexpr int kLeftY = 1;
    static constexpr int kRightY = 5;
    static constexpr int kLeftTrigger = 2;
    static constexpr int kRightTrigger = 3;
  };
};

}  // namespace frc
