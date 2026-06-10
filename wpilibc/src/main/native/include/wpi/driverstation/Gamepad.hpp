// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/driverstation/GenericHID.hpp"
#include "wpi/driverstation/HIDDevice.hpp"
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
class Gamepad : public HIDDevice,
                public wpi::util::Sendable,
                public wpi::util::SendableHelper<Gamepad> {
 public:
  /** Represents a digital button on an Gamepad. */
  enum class Button {
    /// Face Down button.
    FACE_DOWN = 0,
    /// Face Right button.
    FACE_RIGHT = 1,
    /// Face Left button.
    FACE_LEFT = 2,
    /// Face Up button.
    FACE_UP = 3,
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

  /**
   * Construct an instance of a gamepad with a GenericHID object.
   *
   * @param hid The GenericHID object to use for this gamepad.
   */
  explicit Gamepad(GenericHID& hid);

  ~Gamepad() override = default;

  Gamepad(Gamepad&&) = default;
  Gamepad& operator=(Gamepad&&) = default;

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  GenericHID& GetHID() override;

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  const GenericHID& GetHID() const override;

  /**
   * Get the X axis value of left side of the controller. Right is positive.
   *
   * A deadband of 0.1 is applied by default. Use SetLeftXDeadband() to change
   * it.
   *
   * @return the axis value.
   */
  double GetLeftX() const;

  /**
   * Set the deadband for the left X axis.
   *
   * The deadband is clamped to [0, 1).
   *
   * @param deadband The deadband to apply.
   */
  void SetLeftXDeadband(double deadband);

  /**
   * Get the Y axis value of left side of the controller. Back is positive.
   *
   * A deadband of 0.1 is applied by default. Use SetLeftYDeadband() to change
   * it.
   *
   * @return the axis value.
   */
  double GetLeftY() const;

  /**
   * Set the deadband for the left Y axis.
   *
   * The deadband is clamped to [0, 1).
   *
   * @param deadband The deadband to apply.
   */
  void SetLeftYDeadband(double deadband);

  /**
   * Get the X axis value of right side of the controller. Right is positive.
   *
   * A deadband of 0.1 is applied by default. Use SetRightXDeadband() to change
   * it.
   *
   * @return the axis value.
   */
  double GetRightX() const;

  /**
   * Set the deadband for the right X axis.
   *
   * The deadband is clamped to [0, 1).
   *
   * @param deadband The deadband to apply.
   */
  void SetRightXDeadband(double deadband);

  /**
   * Get the Y axis value of right side of the controller. Back is positive.
   *
   * A deadband of 0.1 is applied by default. Use SetRightYDeadband() to change
   * it.
   *
   * @return the axis value.
   */
  double GetRightY() const;

  /**
   * Set the deadband for the right Y axis.
   *
   * The deadband is clamped to [0, 1).
   *
   * @param deadband The deadband to apply.
   */
  void SetRightYDeadband(double deadband);

  /**
   * Get the left trigger axis value of the controller. Note that this axis
   * is bound to the range of [0, 1] as opposed to the usual [-1, 1].
   *
   * A deadband of 0.01 is applied by default. Use SetLeftTriggerDeadband() to
   * change it.
   *
   * @return the axis value.
   */
  double GetLeftTriggerAxis() const;

  /**
   * Set the deadband for the left trigger axis.
   *
   * The deadband is clamped to [0, 1).
   *
   * @param deadband The deadband to apply.
   */
  void SetLeftTriggerDeadband(double deadband);

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
   * A deadband of 0.01 is applied by default. Use SetRightTriggerDeadband() to
   * change it.
   *
   * @return the axis value.
   */
  double GetRightTriggerAxis() const;

  /**
   * Set the deadband for the right trigger axis.
   *
   * The deadband is clamped to [0, 1).
   *
   * @param deadband The deadband to apply.
   */
  void SetRightTriggerDeadband(double deadband);

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
   * Read the value of the Face Down button on the controller.
   *
   * @return The state of the button.
   */
  bool GetFaceDownButton() const;

  /**
   * Whether the Face Down button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetFaceDownButtonPressed();

  /**
   * Whether the Face Down button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetFaceDownButtonReleased();

  /**
   * Constructs an event instance around the Face Down button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Face Down button's
   * digital signal attached to the given loop.
   */
  BooleanEvent FaceDown(EventLoop* loop) const;

  /**
   * Read the value of the Face Right button on the controller.
   *
   * @return The state of the button.
   */
  bool GetFaceRightButton() const;

  /**
   * Whether the Face Right button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetFaceRightButtonPressed();

  /**
   * Whether the Face Right button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetFaceRightButtonReleased();

  /**
   * Constructs an event instance around the Face Right button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Face Right button's
   * digital signal attached to the given loop.
   */
  BooleanEvent FaceRight(EventLoop* loop) const;

  /**
   * Read the value of the Face Left button on the controller.
   *
   * @return The state of the button.
   */
  bool GetFaceLeftButton() const;

  /**
   * Whether the Face Left button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetFaceLeftButtonPressed();

  /**
   * Whether the Face Left button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetFaceLeftButtonReleased();

  /**
   * Constructs an event instance around the Face Left button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Face Left button's
   * digital signal attached to the given loop.
   */
  BooleanEvent FaceLeft(EventLoop* loop) const;

  /**
   * Read the value of the Face Up button on the controller.
   *
   * @return The state of the button.
   */
  bool GetFaceUpButton() const;

  /**
   * Whether the Face Up button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  bool GetFaceUpButtonPressed();

  /**
   * Whether the Face Up button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  bool GetFaceUpButtonReleased();

  /**
   * Constructs an event instance around the Face Up button's
   * digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Face Up button's
   * digital signal attached to the given loop.
   */
  BooleanEvent FaceUp(EventLoop* loop) const;

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

  /**
   * Get the bitmask of axes for the gamepad.
   *
   * @return the number of axis for the current gamepad
   */
  int GetAxesAvailable() const;

  /**
   * For the current gamepad, return the bitmask of available buttons.
   *
   * @return the bitmask of buttons for the current gamepad
   */
  uint64_t GetButtonsAvailable() const;

  /**
   * Get if the gamepad is connected.
   *
   * @return true if the gamepad is connected
   */
  bool IsConnected() const;

  /**
   * Get the type of the gamepad.
   *
   * @return the type of the gamepad.
   */
  GenericHID::HIDType GetGamepadType() const;

  /**
   * Get the supported outputs for the gamepad.
   *
   * @return the supported outputs for the gamepad.
   */
  GenericHID::SupportedOutputs GetSupportedOutputs() const;

  /**
   * Get the name of the gamepad.
   *
   * @return the name of the gamepad.
   */
  std::string GetName() const;

  /**
   * Get the port number of the gamepad.
   *
   * @return The port number of the gamepad.
   */
  int GetPort() const;

  /**
   * Set leds on the gamepad. If only mono is supported, the system will use
   * the highest value passed in.
   *
   * @param r Red value from 0-255
   * @param g Green value from 0-255
   * @param b Blue value from 0-255
   */
  void SetLeds(int r, int g, int b);

  /**
   * Set the rumble output for the HID.
   *
   * The DS currently supports 4 rumble values: left rumble, right rumble, left
   * trigger rumble, and right trigger rumble.
   *
   * @param type  Which rumble value to set
   * @param value The normalized value (0 to 1) to set the rumble to
   */
  void SetRumble(GenericHID::RumbleType type, double value);

  /**
   * Check if a touchpad finger is available.
   * @param touchpad The touchpad to check.
   * @param finger The finger to check.
   * @return true if the touchpad finger is available.
   */
  bool GetTouchpadFingerAvailable(int touchpad, int finger) const;

  /**
   * Get the touchpad finger data.
   * @param touchpad The touchpad to read.
   * @param finger The finger to read.
   * @return The touchpad finger data.
   */
  TouchpadFinger GetTouchpadFinger(int touchpad, int finger) const;

  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  double GetAxisForSendable(Axis axis) const;
  bool GetButtonForSendable(Button button) const;

  double m_leftXDeadband = 0.1;
  double m_leftYDeadband = 0.1;
  double m_rightXDeadband = 0.1;
  double m_rightYDeadband = 0.1;
  double m_leftTriggerDeadband = 0.01;
  double m_rightTriggerDeadband = 0.01;
  GenericHID* m_hid;
};

}  // namespace wpi
