// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/GenericHID.h"

namespace frc {

/**
 * Handle input from PS5 controllers connected to the Driver Station.
 *
 * This class handles PS5 input that comes from the Driver Station. Each time
 * a value is requested the most recent value is returned. There is a single
 * class instance for each controller and the mapping of ports to hardware
 * buttons depends on the code in the Driver Station.
 *
 * Only first party controllers from Sony are guaranteed to have the correct
 * mapping, and only through the official NI DS. Sim is not guaranteed to have
 * the same mapping, as well as any 3rd party controllers.
 */
class PS5Controller : public GenericHID {
 public:
  /**
   * Construct an instance of an PS5 controller.
   *
   * The controller index is the USB port on the Driver Station.
   *
   * @param port The port on the Driver Station that the controller is plugged
   *             into (0-5).
   */
  explicit PS5Controller(int port);

  ~PS5Controller() override = default;

  PS5Controller(PS5Controller&&) = default;
  PS5Controller& operator=(PS5Controller&&) = default;

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return the axis value.
   */
  double GetLeftX() const;

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return the axis value.
   */
  double GetRightX() const;

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return the axis value.
   */
  double GetLeftY() const;

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return the axis value.
   */
  double GetRightY() const;

  /**
   * Get the L2 axis value of the controller. Note that this axis is bound to
   * the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return the axis value.
   */
  double GetL2Axis() const;

  /**
   * Get the R2 axis value of the controller. Note that this axis is bound to
   * the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return the axis value.
   */
  double GetR2Axis() const;

  /**
   * Read the value of the Square button on the controller.
   *
   * @return The state of the button.
   */
  bool GetSquareButton() const;

  /**
   * Whether the Square button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetSquareButtonPressed();

  /**
   * Whether the Square button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetSquareButtonReleased();

  /**
   * Constructs an event instance around the square button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the square button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Square(EventLoop* loop) const;

  /**
   * Read the value of the Cross button on the controller.
   *
   * @return The state of the button.
   */
  bool GetCrossButton() const;

  /**
   * Whether the Cross button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetCrossButtonPressed();

  /**
   * Whether the Cross button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetCrossButtonReleased();

  /**
   * Constructs an event instance around the cross button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the cross button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Cross(EventLoop* loop) const;

  /**
   * Read the value of the Circle button on the controller.
   *
   * @return The state of the button.
   */
  bool GetCircleButton() const;

  /**
   * Whether the Circle button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetCircleButtonPressed();

  /**
   * Whether the Circle button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetCircleButtonReleased();

  /**
   * Constructs an event instance around the circle button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the circle button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Circle(EventLoop* loop) const;

  /**
   * Read the value of the Triangle button on the controller.
   *
   * @return The state of the button.
   */
  bool GetTriangleButton() const;

  /**
   * Whether the Triangle button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetTriangleButtonPressed();

  /**
   * Whether the Triangle button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetTriangleButtonReleased();

  /**
   * Constructs an event instance around the triangle button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the triangle button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Triangle(EventLoop* loop) const;

  /**
   * Read the value of the L1 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetL1Button() const;

  /**
   * Whether the L1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetL1ButtonPressed();

  /**
   * Whether the L1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetL1ButtonReleased();

  /**
   * Constructs an event instance around the L1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L1 button's digital signal
   * attached to the given loop.
   */
  BooleanEvent L1(EventLoop* loop) const;

  /**
   * Read the value of the R1 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetR1Button() const;

  /**
   * Whether the R1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetR1ButtonPressed();

  /**
   * Whether the R1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetR1ButtonReleased();

  /**
   * Constructs an event instance around the R1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R1 button's digital signal
   * attached to the given loop.
   */
  BooleanEvent R1(EventLoop* loop) const;

  /**
   * Read the value of the L2 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetL2Button() const;

  /**
   * Whether the L2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetL2ButtonPressed();

  /**
   * Whether the L2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetL2ButtonReleased();

  /**
   * Constructs an event instance around the L2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L2 button's digital signal
   * attached to the given loop.
   */
  BooleanEvent L2(EventLoop* loop) const;

  /**
   * Read the value of the R2 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetR2Button() const;

  /**
   * Whether the R2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetR2ButtonPressed();

  /**
   * Whether the R2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetR2ButtonReleased();

  /**
   * Constructs an event instance around the R2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R2 button's digital signal
   * attached to the given loop.
   */
  BooleanEvent R2(EventLoop* loop) const;

  /**
   * Read the value of the Create button on the controller.
   *
   * @return The state of the button.
   */
  bool GetCreateButton() const;

  /**
   * Whether the Create button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetCreateButtonPressed();

  /**
   * Whether the Create button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetCreateButtonReleased();

  /**
   * Constructs an event instance around the Create button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Create button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Create(EventLoop* loop) const;

  /**
   * Read the value of the Options button on the controller.
   *
   * @return The state of the button.
   */
  bool GetOptionsButton() const;

  /**
   * Whether the Options button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetOptionsButtonPressed();

  /**
   * Whether the Options button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetOptionsButtonReleased();

  /**
   * Constructs an event instance around the options button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the options button's digital signal
   * attached to the given loop.
   */
  BooleanEvent Options(EventLoop* loop) const;

  /**
   * Read the value of the L3 button (pressing the left analog stick) on the
   * controller.
   *
   * @return The state of the button.
   */
  bool GetL3Button() const;

  /**
   * Whether the L3 (left stick) button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetL3ButtonPressed();

  /**
   * Whether the L3 (left stick) button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetL3ButtonReleased();

  /**
   * Constructs an event instance around the L3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L3 button's digital signal
   * attached to the given loop.
   */
  BooleanEvent L3(EventLoop* loop) const;

  /**
   * Read the value of the R3 button (pressing the right analog stick) on the
   * controller.
   *
   * @return The state of the button.
   */
  bool GetR3Button() const;

  /**
   * Whether the R3 (right stick) button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetR3ButtonPressed();

  /**
   * Whether the R3 (right stick) button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetR3ButtonReleased();

  /**
   * Constructs an event instance around the R3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R3 button's digital signal
   * attached to the given loop.
   */
  BooleanEvent R3(EventLoop* loop) const;

  /**
   * Read the value of the PS button on the controller.
   *
   * @return The state of the button.
   */
  bool GetPSButton() const;

  /**
   * Whether the PS button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetPSButtonPressed();

  /**
   * Whether the PS button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetPSButtonReleased();

  /**
   * Constructs an event instance around the PS button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the PS button's digital signal
   * attached to the given loop.
   */
  BooleanEvent PS(EventLoop* loop) const;

  /**
   * Read the value of the touchpad button on the controller.
   *
   * @return The state of the button.
   */
  bool GetTouchpad() const;

  /**
   * Whether the touchpad was pressed since the last check.
   *
   * @return Whether the touchpad was pressed since the last check.
   */
  bool GetTouchpadPressed();

  /**
   * Whether the touchpad was released since the last check.
   *
   * @return Whether the touchpad was released since the last check.
   */
  bool GetTouchpadReleased();

  /**
   * Constructs an event instance around the touchpad's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the touchpad's digital signal
   * attached to the given loop.
   */
  BooleanEvent Touchpad(EventLoop* loop) const;

  /**
   * Represents a digital button on a PS5Controller.
   */
  struct Button {
    /// Square button.
    static constexpr int kSquare = 1;
    /// X button.
    static constexpr int kCross = 2;
    /// Circle button.
    static constexpr int kCircle = 3;
    /// Triangle button.
    static constexpr int kTriangle = 4;
    /// Left trigger 1 button.
    static constexpr int kL1 = 5;
    /// Right trigger 1 button.
    static constexpr int kR1 = 6;
    /// Left trigger 2 button.
    static constexpr int kL2 = 7;
    /// Right trigger 2 button.
    static constexpr int kR2 = 8;
    /// Create button.
    static constexpr int kCreate = 9;
    /// Options button.
    static constexpr int kOptions = 10;
    /// Left stick button.
    static constexpr int kL3 = 11;
    /// Right stick button.
    static constexpr int kR3 = 12;
    /// PlayStation button.
    static constexpr int kPS = 13;
    /// Touchpad click button.
    static constexpr int kTouchpad = 14;
  };

  /**
   * Represents an axis on a PS5Controller.
   */
  struct Axis {
    /// Left X axis.
    static constexpr int kLeftX = 0;
    /// Left Y axis.
    static constexpr int kLeftY = 1;
    /// Right X axis.
    static constexpr int kRightX = 2;
    /// Right Y axis.
    static constexpr int kRightY = 5;
    /// Left Trigger 2.
    static constexpr int kL2 = 3;
    /// Right Trigger 2.
    static constexpr int kR2 = 4;
  };
};

}  // namespace frc
