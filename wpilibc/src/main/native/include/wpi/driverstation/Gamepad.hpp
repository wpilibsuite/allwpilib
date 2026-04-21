// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/driverstation/GenericHID.hpp"
#include "wpi/util/sendable/Sendable.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

namespace wpi {

/**
 * Handle input from Gamepad controllers connected to the Driver Station.
 *
 * This class handles Gamepad input that comes from the Driver Station. Each
 * time a value is requested the most recent value is returned. There is a
 * single class instance for each controller and the mapping of ports to
 * hardware buttons depends on the code in the Driver Station.
 *
 * Only first party controllers from Generic are guaranteed to have the
 * correct mapping, and only through the official NI DS. Sim is not guaranteed
 * to have the same mapping, as well as any 3rd party controllers.
 */
class Gamepad : public GenericHID,
                public wpi::util::Sendable,
                public wpi::util::SendableHelper<Gamepad> {
 public:
  /** Represents a digital button on an Gamepad. */
  enum class Button {
    /// South Face button.
    SOUTH_FACE = 0,
    /// East Face button.
    EAST_FACE = 1,
    /// West Face button.
    WEST_FACE = 2,
    /// North Face button.
    NORTH_FACE = 3,
    /// Back button.
    BACK = 4,
    /// Guide button.
    GUIDE = 5,
    /// Start button.
    START = 6,
    /// Left stick button.
    LEFT_STICK = 7,
    /// Right stick button.
    RIGHT_STICK = 8,
    /// Left bumper button.
    LEFT_BUMPER = 9,
    /// Right bumper button.
    RIGHT_BUMPER = 10,
    /// D-pad up button.
    DPAD_UP = 11,
    /// D-pad down button.
    DPAD_DOWN = 12,
    /// D-pad left button.
    DPAD_LEFT = 13,
    /// D-pad right button.
    DPAD_RIGHT = 14,
    /// Miscellaneous 1 button.
    MISC_1 = 15,
    /// Right Paddle 1 button.
    RIGHT_PADDLE_1 = 16,
    /// Left Paddle 1 button.
    LEFT_PADDLE_1 = 17,
    /// Right Paddle 2 button.
    RIGHT_PADDLE_2 = 18,
    /// Left Paddle 2 button.
    LEFT_PADDLE_2 = 19,
    /// Touchpad button.
    TOUCHPAD = 20,
    /// Miscellaneous 2 button.
    MISC_2 = 21,
    /// Miscellaneous 3 button.
    MISC_3 = 22,
    /// Miscellaneous 4 button.
    MISC_4 = 23,
    /// Miscellaneous 5 button.
    MISC_5 = 24,
    /// Miscellaneous 6 button.
    MISC_6 = 25,
  };

  /** Represents an axis on an Gamepad. */
  enum class Axis {
    /// Left X axis.
    LEFT_X = 0,
    /// Left Y axis.
    LEFT_Y = 1,
    /// Right X axis.
    RIGHT_X = 2,
    /// Right Y axis.
    RIGHT_Y = 3,
    /// Left trigger.
    LEFT_TRIGGER = 4,
    /// Right trigger.
    RIGHT_TRIGGER = 5,
  };

  /**
   * Construct an instance of a controller.
   *
   * The controller index is the USB port on the Driver Station.
   *
   * @param port The port on the Driver Station that the controller is plugged
   *             into (0-5).
   */
  explicit Gamepad(int port);

  ~Gamepad() override = default;

  Gamepad(Gamepad&&) = default;
  Gamepad& operator=(Gamepad&&) = default;

  /**
   * Get the X axis value of left side of the controller. Right is positive.
   *
   * @return the axis value.
   */
  double GetLeftX() const;

  /**
   * Get the Y axis value of left side of the controller. Back is positive.
   *
   * @return the axis value.
   */
  double GetLeftY() const;

  /**
   * Get the X axis value of right side of the controller. Right is positive.
   *
   * @return the axis value.
   */
  double GetRightX() const;

  /**
   * Get the Y axis value of right side of the controller. Back is positive.
   *
   * @return the axis value.
   */
  double GetRightY() const;

  /**
   * Get the left trigger axis value of the controller. Note that this axis
   * is bound to the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return the axis value.
   */
  double GetLeftTriggerAxis() const;

  /**
   * Constructs an event instance around the axis value of the left trigger.
   * The returned trigger will be true when the axis value is greater than
   * {@code threshold}.
   * @param threshold the minimum axis value for the returned event to be true.
   * This value should be in the range [0, 1] where 0 is the unpressed state of
   * the axis.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the left trigger's axis
   * exceeds the provided threshold, attached to the given event loop
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
   * Get the right trigger axis value of the controller. Note that this axis
   * is bound to the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return the axis value.
   */
  double GetRightTriggerAxis() const;

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

  /**
   * Read the value of the South Face button on the controller.
   *
   * @return The state of the button.
   */
  bool GetSouthFaceButton() const;

  /**
   * Whether the South Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetSouthFaceButtonPressed();

  /**
   * Whether the South Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetSouthFaceButtonReleased();

  /**
   * Constructs an event instance around the South Face button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the South Face button's
   * digital signal attached to the given loop.
   */
  BooleanEvent SouthFace(EventLoop* loop) const;

  /**
   * Read the value of the East Face button on the controller.
   *
   * @return The state of the button.
   */
  bool GetEastFaceButton() const;

  /**
   * Whether the East Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetEastFaceButtonPressed();

  /**
   * Whether the East Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetEastFaceButtonReleased();

  /**
   * Constructs an event instance around the East Face button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the East Face button's
   * digital signal attached to the given loop.
   */
  BooleanEvent EastFace(EventLoop* loop) const;

  /**
   * Read the value of the West Face button on the controller.
   *
   * @return The state of the button.
   */
  bool GetWestFaceButton() const;

  /**
   * Whether the West Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetWestFaceButtonPressed();

  /**
   * Whether the West Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetWestFaceButtonReleased();

  /**
   * Constructs an event instance around the West Face button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the West Face button's
   * digital signal attached to the given loop.
   */
  BooleanEvent WestFace(EventLoop* loop) const;

  /**
   * Read the value of the North Face button on the controller.
   *
   * @return The state of the button.
   */
  bool GetNorthFaceButton() const;

  /**
   * Whether the North Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetNorthFaceButtonPressed();

  /**
   * Whether the North Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetNorthFaceButtonReleased();

  /**
   * Constructs an event instance around the North Face button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the North Face button's
   * digital signal attached to the given loop.
   */
  BooleanEvent NorthFace(EventLoop* loop) const;

  /**
   * Read the value of the Back button on the controller.
   *
   * @return The state of the button.
   */
  bool GetBackButton() const;

  /**
   * Whether the Back button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetBackButtonPressed();

  /**
   * Whether the Back button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetBackButtonReleased();

  /**
   * Constructs an event instance around the Back button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Back button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Back(EventLoop* loop) const;

  /**
   * Read the value of the Guide button on the controller.
   *
   * @return The state of the button.
   */
  bool GetGuideButton() const;

  /**
   * Whether the Guide button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetGuideButtonPressed();

  /**
   * Whether the Guide button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetGuideButtonReleased();

  /**
   * Constructs an event instance around the Guide button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Guide button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Guide(EventLoop* loop) const;

  /**
   * Read the value of the Start button on the controller.
   *
   * @return The state of the button.
   */
  bool GetStartButton() const;

  /**
   * Whether the Start button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetStartButtonPressed();

  /**
   * Whether the Start button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetStartButtonReleased();

  /**
   * Constructs an event instance around the Start button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Start button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Start(EventLoop* loop) const;

  /**
   * Read the value of the left stick button on the controller.
   *
   * @return The state of the button.
   */
  bool GetLeftStickButton() const;

  /**
   * Whether the left stick button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetLeftStickButtonPressed();

  /**
   * Whether the left stick button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetLeftStickButtonReleased();

  /**
   * Constructs an event instance around the left stick button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left stick button's
   * digital signal attached to the given loop.
   */
  BooleanEvent LeftStick(EventLoop* loop) const;

  /**
   * Read the value of the right stick button on the controller.
   *
   * @return The state of the button.
   */
  bool GetRightStickButton() const;

  /**
   * Whether the right stick button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetRightStickButtonPressed();

  /**
   * Whether the right stick button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetRightStickButtonReleased();

  /**
   * Constructs an event instance around the right stick button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right stick button's
   * digital signal attached to the given loop.
   */
  BooleanEvent RightStick(EventLoop* loop) const;

  /**
   * Read the value of the right bumper button on the controller.
   *
   * @return The state of the button.
   */
  bool GetLeftBumperButton() const;

  /**
   * Whether the right bumper button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetLeftBumperButtonPressed();

  /**
   * Whether the right bumper button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetLeftBumperButtonReleased();

  /**
   * Constructs an event instance around the right bumper button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper button's
   * digital signal attached to the given loop.
   */
  BooleanEvent LeftBumper(EventLoop* loop) const;

  /**
   * Read the value of the right bumper button on the controller.
   *
   * @return The state of the button.
   */
  bool GetRightBumperButton() const;

  /**
   * Whether the right bumper button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetRightBumperButtonPressed();

  /**
   * Whether the right bumper button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetRightBumperButtonReleased();

  /**
   * Constructs an event instance around the right bumper button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper button's
   * digital signal attached to the given loop.
   */
  BooleanEvent RightBumper(EventLoop* loop) const;

  /**
   * Read the value of the D-pad up button on the controller.
   *
   * @return The state of the button.
   */
  bool GetDpadUpButton() const;

  /**
   * Whether the D-pad up button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetDpadUpButtonPressed();

  /**
   * Whether the D-pad up button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetDpadUpButtonReleased();

  /**
   * Constructs an event instance around the D-pad up button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad up button's
   * digital signal attached to the given loop.
   */
  BooleanEvent DpadUp(EventLoop* loop) const;

  /**
   * Read the value of the D-pad down button on the controller.
   *
   * @return The state of the button.
   */
  bool GetDpadDownButton() const;

  /**
   * Whether the D-pad down button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetDpadDownButtonPressed();

  /**
   * Whether the D-pad down button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetDpadDownButtonReleased();

  /**
   * Constructs an event instance around the D-pad down button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad down button's
   * digital signal attached to the given loop.
   */
  BooleanEvent DpadDown(EventLoop* loop) const;

  /**
   * Read the value of the D-pad left button on the controller.
   *
   * @return The state of the button.
   */
  bool GetDpadLeftButton() const;

  /**
   * Whether the D-pad left button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetDpadLeftButtonPressed();

  /**
   * Whether the D-pad left button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetDpadLeftButtonReleased();

  /**
   * Constructs an event instance around the D-pad left button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad left button's
   * digital signal attached to the given loop.
   */
  BooleanEvent DpadLeft(EventLoop* loop) const;

  /**
   * Read the value of the D-pad right button on the controller.
   *
   * @return The state of the button.
   */
  bool GetDpadRightButton() const;

  /**
   * Whether the D-pad right button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetDpadRightButtonPressed();

  /**
   * Whether the D-pad right button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetDpadRightButtonReleased();

  /**
   * Constructs an event instance around the D-pad right button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad right button's
   * digital signal attached to the given loop.
   */
  BooleanEvent DpadRight(EventLoop* loop) const;

  /**
   * Read the value of the Miscellaneous 1 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetMisc1Button() const;

  /**
   * Whether the Miscellaneous 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetMisc1ButtonPressed();

  /**
   * Whether the Miscellaneous 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetMisc1ButtonReleased();

  /**
   * Constructs an event instance around the Miscellaneous 1 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 1 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Misc1(EventLoop* loop) const;

  /**
   * Read the value of the Right Paddle 1 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetRightPaddle1Button() const;

  /**
   * Whether the Right Paddle 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetRightPaddle1ButtonPressed();

  /**
   * Whether the Right Paddle 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetRightPaddle1ButtonReleased();

  /**
   * Constructs an event instance around the Right Paddle 1 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Right Paddle 1 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent RightPaddle1(EventLoop* loop) const;

  /**
   * Read the value of the Left Paddle 1 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetLeftPaddle1Button() const;

  /**
   * Whether the Left Paddle 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetLeftPaddle1ButtonPressed();

  /**
   * Whether the Left Paddle 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetLeftPaddle1ButtonReleased();

  /**
   * Constructs an event instance around the Left Paddle 1 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Left Paddle 1 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent LeftPaddle1(EventLoop* loop) const;

  /**
   * Read the value of the Right Paddle 2 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetRightPaddle2Button() const;

  /**
   * Whether the Right Paddle 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetRightPaddle2ButtonPressed();

  /**
   * Whether the Right Paddle 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetRightPaddle2ButtonReleased();

  /**
   * Constructs an event instance around the Right Paddle 2 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Right Paddle 2 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent RightPaddle2(EventLoop* loop) const;

  /**
   * Read the value of the Left Paddle 2 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetLeftPaddle2Button() const;

  /**
   * Whether the Left Paddle 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetLeftPaddle2ButtonPressed();

  /**
   * Whether the Left Paddle 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetLeftPaddle2ButtonReleased();

  /**
   * Constructs an event instance around the Left Paddle 2 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Left Paddle 2 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent LeftPaddle2(EventLoop* loop) const;

  /**
   * Read the value of the Touchpad button on the controller.
   *
   * @return The state of the button.
   */
  bool GetTouchpadButton() const;

  /**
   * Whether the Touchpad button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetTouchpadButtonPressed();

  /**
   * Whether the Touchpad button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetTouchpadButtonReleased();

  /**
   * Constructs an event instance around the Touchpad button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Touchpad button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Touchpad(EventLoop* loop) const;

  /**
   * Read the value of the Miscellaneous 2 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetMisc2Button() const;

  /**
   * Whether the Miscellaneous 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetMisc2ButtonPressed();

  /**
   * Whether the Miscellaneous 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetMisc2ButtonReleased();

  /**
   * Constructs an event instance around the Miscellaneous 2 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 2 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Misc2(EventLoop* loop) const;

  /**
   * Read the value of the Miscellaneous 3 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetMisc3Button() const;

  /**
   * Whether the Miscellaneous 3 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetMisc3ButtonPressed();

  /**
   * Whether the Miscellaneous 3 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetMisc3ButtonReleased();

  /**
   * Constructs an event instance around the Miscellaneous 3 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 3 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Misc3(EventLoop* loop) const;

  /**
   * Read the value of the Miscellaneous 4 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetMisc4Button() const;

  /**
   * Whether the Miscellaneous 4 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetMisc4ButtonPressed();

  /**
   * Whether the Miscellaneous 4 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetMisc4ButtonReleased();

  /**
   * Constructs an event instance around the Miscellaneous 4 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 4 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Misc4(EventLoop* loop) const;

  /**
   * Read the value of the Miscellaneous 5 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetMisc5Button() const;

  /**
   * Whether the Miscellaneous 5 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetMisc5ButtonPressed();

  /**
   * Whether the Miscellaneous 5 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetMisc5ButtonReleased();

  /**
   * Constructs an event instance around the Miscellaneous 5 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 5 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Misc5(EventLoop* loop) const;

  /**
   * Read the value of the Miscellaneous 6 button on the controller.
   *
   * @return The state of the button.
   */
  bool GetMisc6Button() const;

  /**
   * Whether the Miscellaneous 6 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetMisc6ButtonPressed();

  /**
   * Whether the Miscellaneous 6 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetMisc6ButtonReleased();

  /**
   * Constructs an event instance around the Miscellaneous 6 button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 6 button's
   * digital signal attached to the given loop.
   */
  BooleanEvent Misc6(EventLoop* loop) const;

  /**
   * Get the button value.
   *
   * This method returns true if the button is being held down at the time
   * that this method is being called.
   *
   * @param button The button
   * @return The state of the button.
   */
  bool GetButton(Button button) const;

  /**
   * Whether the button was pressed since the last check.
   *
   * This method returns true if the button went from not pressed to held down
   * since the last time this method was called. This is useful if you only
   * want to call a function once when you press the button.
   *
   * @param button The button
   * @return Whether the button was pressed since the last check.
   */
  bool GetButtonPressed(Button button);

  /**
   * Whether the button was released since the last check.
   *
   * This method returns true if the button went from held down to not pressed
   * since the last time this method was called. This is useful if you only
   * want to call a function once when you release the button.
   *
   * @param button The button
   * @return Whether the button was released since the last check.
   */
  bool GetButtonReleased(Button button);

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached
   * to the given loop.
   */
  BooleanEvent ButtonEvent(Button button, EventLoop* loop) const;

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read
   * @return The value of the axis.
   */
  double GetAxis(Axis axis) const;

  /**
   * Constructs an event instance that is true when the axis value is less than
   * threshold
   *
   * @param axis The axis to read
   * @param threshold The value below which this trigger should return true.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the axis value is less than the
   * provided threshold.
   */
  BooleanEvent AxisLessThan(Axis axis, double threshold, EventLoop* loop) const;

  /**
   * Constructs an event instance that is true when the axis value is greater
   * than threshold
   *
   * @param axis The axis to read
   * @param threshold The value above which this trigger should return true.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the axis value is greater than
   * the provided threshold.
   */
  BooleanEvent AxisGreaterThan(Axis axis, double threshold,
                               EventLoop* loop) const;

  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  double GetAxisForSendable(Axis axis) const;
  bool GetButtonForSendable(Button button) const;
};

}  // namespace wpi
