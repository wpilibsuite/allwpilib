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
 *
 * Only first party controllers from Microsoft are guaranteed to have the
 * correct mapping, and only through the official NI DS. Sim is not guaranteed
 * to have the same mapping, as well as any 3rd party controllers.
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
   * Get the left trigger (LT) axis value of the controller. Note that this axis
   * is bound to the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return the axis value
   */
  double GetLeftTriggerAxis() const;

  /**
   * Get the right trigger (RT) axis value of the controller. Note that this
   * axis is bound to the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return the axis value
   */
  double GetRightTriggerAxis() const;

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
   * Read the value of the back button on the controller.
   *
   * @return The state of the button.
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
   * Constructs an event instance around the back button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the back button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Back(EventLoop* loop) const;

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

  /**
   * Constructs an event instance around the start button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the start button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Start(EventLoop* loop) const;

  /**
   * Constructs an event instance around the axis value of the left trigger. The
   * returned trigger will be true when the axis value is greater than {@code
   * threshold}.
   * @param threshold the minimum axis value for the returned event to be true.
   * This value should be in the range [0, 1] where 0 is the unpressed state of
   * the axis.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the left trigger's axis exceeds
   * the provided threshold, attached to the given event loop
   */
  BooleanEvent LeftTrigger(double threshold, EventLoop* loop) const;

  /**
   * Constructs an event instance around the axis value of the left trigger.
   * The returned trigger will be true when the axis value is greater than 0.5.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the left trigger's axis
   * exceeds 0.5, attached to the given event loop
   */
  BooleanEvent LeftTrigger(EventLoop* loop) const;

  /**
   * Constructs an event instance around the axis value of the right trigger.
   * The returned trigger will be true when the axis value is greater than
   * {@code threshold}.
   * @param threshold the minimum axis value for the returned event to be true.
   * This value should be in the range [0, 1] where 0 is the unpressed state of
   * the axis.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the right trigger's axis
   * exceeds the provided threshold, attached to the given event loop
   */
  BooleanEvent RightTrigger(double threshold, EventLoop* loop) const;

  /**
   * Constructs an event instance around the axis value of the right trigger.
   * The returned trigger will be true when the axis value is greater than 0.5.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the right trigger's axis
   * exceeds 0.5, attached to the given event loop
   */
  BooleanEvent RightTrigger(EventLoop* loop) const;

  /** Represents a digital button on an XboxController. */
  struct Button {
    /// Left bumper.
    static constexpr int kLeftBumper = 5;
    /// Right bumper.
    static constexpr int kRightBumper = 6;
    /// Left stick.
    static constexpr int kLeftStick = 9;
    /// Right stick.
    static constexpr int kRightStick = 10;
    /// A.
    static constexpr int kA = 1;
    /// B.
    static constexpr int kB = 2;
    /// X.
    static constexpr int kX = 3;
    /// Y.
    static constexpr int kY = 4;
    /// Back.
    static constexpr int kBack = 7;
    /// Start.
    static constexpr int kStart = 8;
  };

  /** Represents an axis on an XboxController. */
  struct Axis {
    /// Left X.
    static constexpr int kLeftX = 0;
    /// Right X.
    static constexpr int kRightX = 4;
    /// Left Y.
    static constexpr int kLeftY = 1;
    /// Right Y.
    static constexpr int kRightY = 5;
    /// Left trigger.
    static constexpr int kLeftTrigger = 2;
    /// Right trigger.
    static constexpr int kRightTrigger = 3;
  };
};

}  // namespace frc
