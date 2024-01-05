// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/GenericHID.h"

namespace frc {

/**
 * Handle input from Stadia controllers connected to the Driver
 * Station.
 *
 * This class handles Stadia input that comes from the Driver Station. Each time
 * a value is requested the most recent value is returned. There is a single
 * class instance for each controller and the mapping of ports to hardware
 * buttons depends on the code in the Driver Station.
 */
class StadiaController : public GenericHID {
 public:
  /**
   * Construct an instance of a Stadia controller.
   *
   * The controller index is the USB port on the Driver Station.
   *
   * @param port The port on the Driver Station that the controller is plugged
   *             into (0-5).
   */
  explicit StadiaController(int port);

  ~StadiaController() override = default;

  StadiaController(StadiaController&&) = default;
  StadiaController& operator=(StadiaController&&) = default;

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return the axis value
   */
  double GetLeftX() const;

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return the axis value
   */
  double GetRightX() const;

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return the axis value
   */
  double GetLeftY() const;

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return the axis value
   */
  double GetRightY() const;

  /**
   * Read the value of the left bumper (LB) button on the controller.
   *
   * @return the state of the button
   */
  bool GetLeftBumper() const;

  /**
   * Read the value of the right bumper (RB) button on the controller.
   *
   * @return the state of the button
   */
  bool GetRightBumper() const;

  /**
   * Whether the left bumper (LB) was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check
   */
  bool GetLeftBumperPressed();

  /**
   * Whether the right bumper (RB) was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check
   */
  bool GetRightBumperPressed();

  /**
   * Whether the left bumper (LB) was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetLeftBumperReleased();

  /**
   * Whether the right bumper (RB) was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetRightBumperReleased();

  /**
   * Constructs an event instance around the left bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left bumper's digital signal
   * attached to the given loop.
   */
  BooleanEvent LeftBumper(EventLoop* loop) const;

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper's digital signal
   * attached to the given loop.
   */
  BooleanEvent RightBumper(EventLoop* loop) const;

  /**
   * Read the value of the left stick button (LSB) on the controller.
   *
   * @return the state of the button
   */
  bool GetLeftStickButton() const;

  /**
   * Read the value of the right stick button (RSB) on the controller.
   *
   * @return the state of the button
   */
  bool GetRightStickButton() const;

  /**
   * Whether the left stick button (LSB) was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetLeftStickButtonPressed();

  /**
   * Whether the right stick button (RSB) was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check
   */
  bool GetRightStickButtonPressed();

  /**
   * Whether the left stick button (LSB) was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetLeftStickButtonReleased();

  /**
   * Whether the right stick button (RSB) was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetRightStickButtonReleased();

  /**
   * Constructs an event instance around the left stick's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left stick's digital signal
   * attached to the given loop.
   */
  BooleanEvent LeftStick(EventLoop* loop) const;

  /**
   * Constructs an event instance around the right stick's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right stick's digital signal
   * attached to the given loop.
   */
  BooleanEvent RightStick(EventLoop* loop) const;

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
   * Constructs an event instance around the A button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the A button's digital signal
   * attached to the given loop.
   */
  BooleanEvent A(EventLoop* loop) const;

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
   * Constructs an event instance around the B button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the B button's digital signal
   * attached to the given loop.
   */
  BooleanEvent B(EventLoop* loop) const;

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
   * Constructs an event instance around the X button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the X button's digital signal
   * attached to the given loop.
   */
  BooleanEvent X(EventLoop* loop) const;

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
   * Constructs an event instance around the Y button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Y button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Y(EventLoop* loop) const;

  /**
   * Read the value of the ellipses button on the controller.
   *
   * @return The state of the button.
   */
  bool GetEllipsesButton() const;

  /**
   * Whether the ellipses button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetEllipsesButtonPressed();

  /**
   * Whether the ellipses button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetEllipsesButtonReleased();

  /**
   * Constructs an event instance around the ellipses button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the ellipses button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Ellipses(EventLoop* loop) const;

  /**
   * Read the value of the hamburger button on the controller.
   *
   * @return The state of the button.
   */
  bool GetHamburgerButton() const;

  /**
   * Whether the hamburger button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetHamburgerButtonPressed();

  /**
   * Whether the hamburger button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetHamburgerButtonReleased();

  /**
   * Constructs an event instance around the hamburger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the hamburger button's digital
   * signal attached to the given loop.
   */
  BooleanEvent Hamburger(EventLoop* loop) const;

  /**
   * Read the value of the stadia button on the controller.
   *
   * @return The state of the button.
   */
  bool GetStadiaButton() const;

  /**
   * Whether the stadia button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetStadiaButtonPressed();

  /**
   * Whether the stadia button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetStadiaButtonReleased();

  /**
   * Constructs an event instance around the stadia button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the stadia button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Stadia(EventLoop* loop) const;

  /**
   * Read the value of the google button on the controller.
   *
   * @return The state of the button.
   */
  bool GetGoogleButton() const;

  /**
   * Whether the google button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetGoogleButtonPressed();

  /**
   * Whether the google button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetGoogleButtonReleased();

  /**
   * Constructs an event instance around the google button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the google button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Google(EventLoop* loop) const;

  /**
   * Read the value of the frame button on the controller.
   *
   * @return The state of the button.
   */
  bool GetFrameButton() const;

  /**
   * Whether the frame button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetFrameButtonPressed();

  /**
   * Whether the frame button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetFrameButtonReleased();

  /**
   * Constructs an event instance around the frame button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the frame button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Frame(EventLoop* loop) const;

  /**
   * Read the value of the left trigger button on the controller.
   *
   * @return The state of the button.
   */
  bool GetLeftTriggerButton() const;

  /**
   * Whether the left trigger button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetLeftTriggerButtonPressed();

  /**
   * Whether the left trigger button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetLeftTriggerButtonReleased();

  /**
   * Constructs an event instance around the left trigger button's digital
   * signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left trigger button's digital
   * signal attached to the given loop.
   */
  BooleanEvent LeftTrigger(EventLoop* loop) const;

  /**
   * Read the value of the right trigger button on the controller.
   *
   * @return The state of the button.
   */
  bool GetRightTriggerButton() const;

  /**
   * Whether the right trigger button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetRightTriggerButtonPressed();

  /**
   * Whether the right trigger button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetRightTriggerButtonReleased();

  /**
   * Constructs an event instance around the right trigger button's digital
   * signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right trigger button's digital
   * signal attached to the given loop.
   */
  BooleanEvent RightTrigger(EventLoop* loop) const;

  /**
   * Represents a digital button on a StadiaController.
   */
  struct Button {
    /// A button.
    static constexpr int kA = 1;
    /// B button.
    static constexpr int kB = 2;
    /// X button.
    static constexpr int kX = 3;
    /// Y button.
    static constexpr int kY = 4;
    /// Left bumper button.
    static constexpr int kLeftBumper = 5;
    /// Right bumper button.
    static constexpr int kRightBumper = 6;
    /// Left stick button.
    static constexpr int kLeftStick = 7;
    /// Right stick button.
    static constexpr int kRightStick = 8;
    /// Ellipses button.
    static constexpr int kEllipses = 9;
    /// Hamburger button.
    static constexpr int kHamburger = 10;
    /// Stadia button.
    static constexpr int kStadia = 11;
    /// Right trigger button.
    static constexpr int kRightTrigger = 12;
    /// Left trigger button.
    static constexpr int kLeftTrigger = 13;
    /// Google button.
    static constexpr int kGoogle = 14;
    /// Frame button.
    static constexpr int kFrame = 15;
  };

  /**
   * Represents an axis on a StadiaController.
   */
  struct Axis {
    /// Left X axis.
    static constexpr int kLeftX = 0;
    /// Right X axis.
    static constexpr int kRightX = 4;
    /// Left Y axis.
    static constexpr int kLeftY = 1;
    /// Right Y axis.
    static constexpr int kRightY = 5;
  };
};

}  // namespace frc
