// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import org.wpilib.event.BooleanEvent;
import org.wpilib.event.EventLoop;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.util.sendable.Sendable;
import org.wpilib.util.sendable.SendableBuilder;

/**
 * Handle input from Gamepad controllers connected to the Driver Station.
 *
 * <p>This class handles Gamepad input that comes from the Driver Station. Each time a value is
 * requested the most recent value is returned. There is a single class instance for each controller
 * and the mapping of ports to hardware buttons depends on the code in the Driver Station.
 *
 * <p>Only first party controllers from Generic are guaranteed to have the correct mapping, and only
 * through the official NI DS. Sim is not guaranteed to have the same mapping, as well as any 3rd
 * party controllers.
 */
public class Gamepad extends GenericHID implements Sendable {
  /** Represents a digital button on a Gamepad. */
  public enum Button {
    /** South Face button. */
    kSouthFace(0),
    /** East Face button. */
    kEastFace(1),
    /** West Face button. */
    kWestFace(2),
    /** North Face button. */
    kNorthFace(3),
    /** Back button. */
    kBack(4),
    /** Guide button. */
    kGuide(5),
    /** Start button. */
    kStart(6),
    /** Left stick button. */
    kLeftStick(7),
    /** Right stick button. */
    kRightStick(8),
    /** Left bumper button. */
    kLeftBumper(9),
    /** Right bumper button. */
    kRightBumper(10),
    /** D-pad up button. */
    kDpadUp(11),
    /** D-pad down button. */
    kDpadDown(12),
    /** D-pad left button. */
    kDpadLeft(13),
    /** D-pad right button. */
    kDpadRight(14),
    /** Miscellaneous 1 button. */
    kMisc1(15),
    /** Right Paddle 1 button. */
    kRightPaddle1(16),
    /** Left Paddle 1 button. */
    kLeftPaddle1(17),
    /** Right Paddle 2 button. */
    kRightPaddle2(18),
    /** Left Paddle 2 button. */
    kLeftPaddle2(19),
    /** Touchpad button. */
    kTouchpad(20),
    /** Miscellaneous 2 button. */
    kMisc2(21),
    /** Miscellaneous 3 button. */
    kMisc3(22),
    /** Miscellaneous 4 button. */
    kMisc4(23),
    /** Miscellaneous 5 button. */
    kMisc5(24),
    /** Miscellaneous 6 button. */
    kMisc6(25);

    /** Button value. */
    public final int value;

    Button(int value) {
      this.value = value;
    }

    /**
     * Get the human-friendly name of the button, matching the relevant methods. This is done by
     * stripping the leading `k`, and appending `Button`.
     *
     * <p>Primarily used for automated unit tests.
     *
     * @return the human-friendly name of the button.
     */
    @Override
    public String toString() {
      // Remove leading `k`
      return this.name().substring(1) + "Button";
    }
  }

  /** Represents an axis on an Gamepad. */
  public enum Axis {
    /** Left X axis. */
    kLeftX(0),
    /** Left Y axis. */
    kLeftY(1),
    /** Right X axis. */
    kRightX(2),
    /** Right Y axis. */
    kRightY(3),
    /** Left trigger. */
    kLeftTrigger(4),
    /** Right trigger. */
    kRightTrigger(5);

    /** Axis value. */
    public final int value;

    Axis(int value) {
      this.value = value;
    }

    /**
     * Get the human-friendly name of the axis, matching the relevant methods. This is done by
     * stripping the leading `k`, and appending `Axis` if the name ends with `Trigger`.
     *
     * <p>Primarily used for automated unit tests.
     *
     * @return the human-friendly name of the axis.
     */
    @Override
    public String toString() {
      var name = this.name().substring(1); // Remove leading `k`
      if (name.endsWith("Trigger")) {
        return name + "Axis";
      }
      return name;
    }
  }

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into (0-5).
   */
  public Gamepad(final int port) {
    super(port);
    HAL.reportUsage("HID", port, "Gamepad");
  }

  /**
   * Get the X axis value of left side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  public double getLeftX() {
    return getRawAxis(Axis.kLeftX.value);
  }

  /**
   * Get the Y axis value of left side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return getRawAxis(Axis.kLeftY.value);
  }

  /**
   * Get the X axis value of right side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return getRawAxis(Axis.kRightX.value);
  }

  /**
   * Get the Y axis value of right side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return getRawAxis(Axis.kRightY.value);
  }

  /**
   * Get the left trigger axis value of the controller. Note that this axis is bound to the range of
   * [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getLeftTriggerAxis() {
    return getRawAxis(Axis.kLeftTrigger.value);
  }

  /**
   * Constructs an event instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link BooleanEvent} to be true. This
   *     value should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public BooleanEvent leftTrigger(double threshold, EventLoop loop) {
    return axisGreaterThan(Axis.kLeftTrigger.value, threshold, loop);
  }

  /**
   * Constructs an event instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public BooleanEvent leftTrigger(EventLoop loop) {
    return leftTrigger(0.5, loop);
  }

  /**
   * Get the right trigger axis value of the controller. Note that this axis is bound to the range
   * of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getRightTriggerAxis() {
    return getRawAxis(Axis.kRightTrigger.value);
  }

  /**
   * Constructs an event instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link BooleanEvent} to be true. This
   *     value should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public BooleanEvent rightTrigger(double threshold, EventLoop loop) {
    return axisGreaterThan(Axis.kRightTrigger.value, threshold, loop);
  }

  /**
   * Constructs an event instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public BooleanEvent rightTrigger(EventLoop loop) {
    return rightTrigger(0.5, loop);
  }

  /**
   * Read the value of the South Face button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getSouthFaceButton() {
    return getRawButton(Button.kSouthFace.value);
  }

  /**
   * Whether the South Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getSouthFaceButtonPressed() {
    return getRawButtonPressed(Button.kSouthFace.value);
  }

  /**
   * Whether the South Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getSouthFaceButtonReleased() {
    return getRawButtonReleased(Button.kSouthFace.value);
  }

  /**
   * Constructs an event instance around the South Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the South Face button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent southFace(EventLoop loop) {
    return button(Button.kSouthFace.value, loop);
  }

  /**
   * Read the value of the East Face button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getEastFaceButton() {
    return getRawButton(Button.kEastFace.value);
  }

  /**
   * Whether the East Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getEastFaceButtonPressed() {
    return getRawButtonPressed(Button.kEastFace.value);
  }

  /**
   * Whether the East Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getEastFaceButtonReleased() {
    return getRawButtonReleased(Button.kEastFace.value);
  }

  /**
   * Constructs an event instance around the East Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the East Face button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent eastFace(EventLoop loop) {
    return button(Button.kEastFace.value, loop);
  }

  /**
   * Read the value of the West Face button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getWestFaceButton() {
    return getRawButton(Button.kWestFace.value);
  }

  /**
   * Whether the West Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getWestFaceButtonPressed() {
    return getRawButtonPressed(Button.kWestFace.value);
  }

  /**
   * Whether the West Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getWestFaceButtonReleased() {
    return getRawButtonReleased(Button.kWestFace.value);
  }

  /**
   * Constructs an event instance around the West Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the West Face button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent westFace(EventLoop loop) {
    return button(Button.kWestFace.value, loop);
  }

  /**
   * Read the value of the North Face button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getNorthFaceButton() {
    return getRawButton(Button.kNorthFace.value);
  }

  /**
   * Whether the North Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getNorthFaceButtonPressed() {
    return getRawButtonPressed(Button.kNorthFace.value);
  }

  /**
   * Whether the North Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getNorthFaceButtonReleased() {
    return getRawButtonReleased(Button.kNorthFace.value);
  }

  /**
   * Constructs an event instance around the North Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the North Face button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent northFace(EventLoop loop) {
    return button(Button.kNorthFace.value, loop);
  }

  /**
   * Read the value of the Back button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getBackButton() {
    return getRawButton(Button.kBack.value);
  }

  /**
   * Whether the Back button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getBackButtonPressed() {
    return getRawButtonPressed(Button.kBack.value);
  }

  /**
   * Whether the Back button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getBackButtonReleased() {
    return getRawButtonReleased(Button.kBack.value);
  }

  /**
   * Constructs an event instance around the Back button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Back button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent back(EventLoop loop) {
    return button(Button.kBack.value, loop);
  }

  /**
   * Read the value of the Guide button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getGuideButton() {
    return getRawButton(Button.kGuide.value);
  }

  /**
   * Whether the Guide button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getGuideButtonPressed() {
    return getRawButtonPressed(Button.kGuide.value);
  }

  /**
   * Whether the Guide button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getGuideButtonReleased() {
    return getRawButtonReleased(Button.kGuide.value);
  }

  /**
   * Constructs an event instance around the Guide button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Guide button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent guide(EventLoop loop) {
    return button(Button.kGuide.value, loop);
  }

  /**
   * Read the value of the Start button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getStartButton() {
    return getRawButton(Button.kStart.value);
  }

  /**
   * Whether the Start button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getStartButtonPressed() {
    return getRawButtonPressed(Button.kStart.value);
  }

  /**
   * Whether the Start button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getStartButtonReleased() {
    return getRawButtonReleased(Button.kStart.value);
  }

  /**
   * Constructs an event instance around the Start button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Start button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent start(EventLoop loop) {
    return button(Button.kStart.value, loop);
  }

  /**
   * Read the value of the left stick button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftStickButton() {
    return getRawButton(Button.kLeftStick.value);
  }

  /**
   * Whether the left stick button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftStickButtonPressed() {
    return getRawButtonPressed(Button.kLeftStick.value);
  }

  /**
   * Whether the left stick button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftStickButtonReleased() {
    return getRawButtonReleased(Button.kLeftStick.value);
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left stick button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent leftStick(EventLoop loop) {
    return button(Button.kLeftStick.value, loop);
  }

  /**
   * Read the value of the right stick button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightStickButton() {
    return getRawButton(Button.kRightStick.value);
  }

  /**
   * Whether the right stick button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightStickButtonPressed() {
    return getRawButtonPressed(Button.kRightStick.value);
  }

  /**
   * Whether the right stick button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightStickButtonReleased() {
    return getRawButtonReleased(Button.kRightStick.value);
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent rightStick(EventLoop loop) {
    return button(Button.kRightStick.value, loop);
  }

  /**
   * Read the value of the right bumper button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftBumperButton() {
    return getRawButton(Button.kLeftBumper.value);
  }

  /**
   * Whether the right bumper button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftBumperButtonPressed() {
    return getRawButtonPressed(Button.kLeftBumper.value);
  }

  /**
   * Whether the right bumper button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftBumperButtonReleased() {
    return getRawButtonReleased(Button.kLeftBumper.value);
  }

  /**
   * Constructs an event instance around the right bumper button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent leftBumper(EventLoop loop) {
    return button(Button.kLeftBumper.value, loop);
  }

  /**
   * Read the value of the right bumper button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightBumperButton() {
    return getRawButton(Button.kRightBumper.value);
  }

  /**
   * Whether the right bumper button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightBumperButtonPressed() {
    return getRawButtonPressed(Button.kRightBumper.value);
  }

  /**
   * Whether the right bumper button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightBumperButtonReleased() {
    return getRawButtonReleased(Button.kRightBumper.value);
  }

  /**
   * Constructs an event instance around the right bumper button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent rightBumper(EventLoop loop) {
    return button(Button.kRightBumper.value, loop);
  }

  /**
   * Read the value of the D-pad up button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getDpadUpButton() {
    return getRawButton(Button.kDpadUp.value);
  }

  /**
   * Whether the D-pad up button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getDpadUpButtonPressed() {
    return getRawButtonPressed(Button.kDpadUp.value);
  }

  /**
   * Whether the D-pad up button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getDpadUpButtonReleased() {
    return getRawButtonReleased(Button.kDpadUp.value);
  }

  /**
   * Constructs an event instance around the D-pad up button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad up button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent dpadUp(EventLoop loop) {
    return button(Button.kDpadUp.value, loop);
  }

  /**
   * Read the value of the D-pad down button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getDpadDownButton() {
    return getRawButton(Button.kDpadDown.value);
  }

  /**
   * Whether the D-pad down button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getDpadDownButtonPressed() {
    return getRawButtonPressed(Button.kDpadDown.value);
  }

  /**
   * Whether the D-pad down button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getDpadDownButtonReleased() {
    return getRawButtonReleased(Button.kDpadDown.value);
  }

  /**
   * Constructs an event instance around the D-pad down button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad down button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent dpadDown(EventLoop loop) {
    return button(Button.kDpadDown.value, loop);
  }

  /**
   * Read the value of the D-pad left button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getDpadLeftButton() {
    return getRawButton(Button.kDpadLeft.value);
  }

  /**
   * Whether the D-pad left button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getDpadLeftButtonPressed() {
    return getRawButtonPressed(Button.kDpadLeft.value);
  }

  /**
   * Whether the D-pad left button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getDpadLeftButtonReleased() {
    return getRawButtonReleased(Button.kDpadLeft.value);
  }

  /**
   * Constructs an event instance around the D-pad left button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad left button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent dpadLeft(EventLoop loop) {
    return button(Button.kDpadLeft.value, loop);
  }

  /**
   * Read the value of the D-pad right button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getDpadRightButton() {
    return getRawButton(Button.kDpadRight.value);
  }

  /**
   * Whether the D-pad right button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getDpadRightButtonPressed() {
    return getRawButtonPressed(Button.kDpadRight.value);
  }

  /**
   * Whether the D-pad right button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getDpadRightButtonReleased() {
    return getRawButtonReleased(Button.kDpadRight.value);
  }

  /**
   * Constructs an event instance around the D-pad right button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad right button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent dpadRight(EventLoop loop) {
    return button(Button.kDpadRight.value, loop);
  }

  /**
   * Read the value of the Miscellaneous 1 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc1Button() {
    return getRawButton(Button.kMisc1.value);
  }

  /**
   * Whether the Miscellaneous 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc1ButtonPressed() {
    return getRawButtonPressed(Button.kMisc1.value);
  }

  /**
   * Whether the Miscellaneous 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc1ButtonReleased() {
    return getRawButtonReleased(Button.kMisc1.value);
  }

  /**
   * Constructs an event instance around the Miscellaneous 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 1 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc1(EventLoop loop) {
    return button(Button.kMisc1.value, loop);
  }

  /**
   * Read the value of the Right Paddle 1 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightPaddle1Button() {
    return getRawButton(Button.kRightPaddle1.value);
  }

  /**
   * Whether the Right Paddle 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightPaddle1ButtonPressed() {
    return getRawButtonPressed(Button.kRightPaddle1.value);
  }

  /**
   * Whether the Right Paddle 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightPaddle1ButtonReleased() {
    return getRawButtonReleased(Button.kRightPaddle1.value);
  }

  /**
   * Constructs an event instance around the Right Paddle 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Right Paddle 1 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent rightPaddle1(EventLoop loop) {
    return button(Button.kRightPaddle1.value, loop);
  }

  /**
   * Read the value of the Left Paddle 1 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftPaddle1Button() {
    return getRawButton(Button.kLeftPaddle1.value);
  }

  /**
   * Whether the Left Paddle 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftPaddle1ButtonPressed() {
    return getRawButtonPressed(Button.kLeftPaddle1.value);
  }

  /**
   * Whether the Left Paddle 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftPaddle1ButtonReleased() {
    return getRawButtonReleased(Button.kLeftPaddle1.value);
  }

  /**
   * Constructs an event instance around the Left Paddle 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Left Paddle 1 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent leftPaddle1(EventLoop loop) {
    return button(Button.kLeftPaddle1.value, loop);
  }

  /**
   * Read the value of the Right Paddle 2 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightPaddle2Button() {
    return getRawButton(Button.kRightPaddle2.value);
  }

  /**
   * Whether the Right Paddle 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightPaddle2ButtonPressed() {
    return getRawButtonPressed(Button.kRightPaddle2.value);
  }

  /**
   * Whether the Right Paddle 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightPaddle2ButtonReleased() {
    return getRawButtonReleased(Button.kRightPaddle2.value);
  }

  /**
   * Constructs an event instance around the Right Paddle 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Right Paddle 2 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent rightPaddle2(EventLoop loop) {
    return button(Button.kRightPaddle2.value, loop);
  }

  /**
   * Read the value of the Left Paddle 2 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftPaddle2Button() {
    return getRawButton(Button.kLeftPaddle2.value);
  }

  /**
   * Whether the Left Paddle 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftPaddle2ButtonPressed() {
    return getRawButtonPressed(Button.kLeftPaddle2.value);
  }

  /**
   * Whether the Left Paddle 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftPaddle2ButtonReleased() {
    return getRawButtonReleased(Button.kLeftPaddle2.value);
  }

  /**
   * Constructs an event instance around the Left Paddle 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Left Paddle 2 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent leftPaddle2(EventLoop loop) {
    return button(Button.kLeftPaddle2.value, loop);
  }

  /**
   * Read the value of the Touchpad button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getTouchpadButton() {
    return getRawButton(Button.kTouchpad.value);
  }

  /**
   * Whether the Touchpad button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getTouchpadButtonPressed() {
    return getRawButtonPressed(Button.kTouchpad.value);
  }

  /**
   * Whether the Touchpad button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getTouchpadButtonReleased() {
    return getRawButtonReleased(Button.kTouchpad.value);
  }

  /**
   * Constructs an event instance around the Touchpad button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Touchpad button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent touchpad(EventLoop loop) {
    return button(Button.kTouchpad.value, loop);
  }

  /**
   * Read the value of the Miscellaneous 2 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc2Button() {
    return getRawButton(Button.kMisc2.value);
  }

  /**
   * Whether the Miscellaneous 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc2ButtonPressed() {
    return getRawButtonPressed(Button.kMisc2.value);
  }

  /**
   * Whether the Miscellaneous 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc2ButtonReleased() {
    return getRawButtonReleased(Button.kMisc2.value);
  }

  /**
   * Constructs an event instance around the Miscellaneous 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 2 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc2(EventLoop loop) {
    return button(Button.kMisc2.value, loop);
  }

  /**
   * Read the value of the Miscellaneous 3 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc3Button() {
    return getRawButton(Button.kMisc3.value);
  }

  /**
   * Whether the Miscellaneous 3 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc3ButtonPressed() {
    return getRawButtonPressed(Button.kMisc3.value);
  }

  /**
   * Whether the Miscellaneous 3 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc3ButtonReleased() {
    return getRawButtonReleased(Button.kMisc3.value);
  }

  /**
   * Constructs an event instance around the Miscellaneous 3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 3 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc3(EventLoop loop) {
    return button(Button.kMisc3.value, loop);
  }

  /**
   * Read the value of the Miscellaneous 4 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc4Button() {
    return getRawButton(Button.kMisc4.value);
  }

  /**
   * Whether the Miscellaneous 4 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc4ButtonPressed() {
    return getRawButtonPressed(Button.kMisc4.value);
  }

  /**
   * Whether the Miscellaneous 4 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc4ButtonReleased() {
    return getRawButtonReleased(Button.kMisc4.value);
  }

  /**
   * Constructs an event instance around the Miscellaneous 4 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 4 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc4(EventLoop loop) {
    return button(Button.kMisc4.value, loop);
  }

  /**
   * Read the value of the Miscellaneous 5 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc5Button() {
    return getRawButton(Button.kMisc5.value);
  }

  /**
   * Whether the Miscellaneous 5 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc5ButtonPressed() {
    return getRawButtonPressed(Button.kMisc5.value);
  }

  /**
   * Whether the Miscellaneous 5 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc5ButtonReleased() {
    return getRawButtonReleased(Button.kMisc5.value);
  }

  /**
   * Constructs an event instance around the Miscellaneous 5 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 5 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc5(EventLoop loop) {
    return button(Button.kMisc5.value, loop);
  }

  /**
   * Read the value of the Miscellaneous 6 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc6Button() {
    return getRawButton(Button.kMisc6.value);
  }

  /**
   * Whether the Miscellaneous 6 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc6ButtonPressed() {
    return getRawButtonPressed(Button.kMisc6.value);
  }

  /**
   * Whether the Miscellaneous 6 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc6ButtonReleased() {
    return getRawButtonReleased(Button.kMisc6.value);
  }

  /**
   * Constructs an event instance around the Miscellaneous 6 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 6 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc6(EventLoop loop) {
    return button(Button.kMisc6.value, loop);
  }

  private double getAxisForSendable(int axis) {
    return DriverStation.getStickAxisIfAvailable(getPort(), axis).orElse(0.0);
  }

  private boolean getButtonForSendable(int button) {
    return DriverStation.getStickButtonIfAvailable(getPort(), button).orElse(false);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("HID");
    builder.publishConstString("ControllerType", "Gamepad");
    builder.addDoubleProperty(
        "LeftTrigger Axis", () -> getAxisForSendable(Axis.kLeftTrigger.value), null);
    builder.addDoubleProperty(
        "RightTrigger Axis", () -> getAxisForSendable(Axis.kRightTrigger.value), null);
    builder.addDoubleProperty("LeftX", () -> getAxisForSendable(Axis.kLeftX.value), null);
    builder.addDoubleProperty("LeftY", () -> getAxisForSendable(Axis.kLeftY.value), null);
    builder.addDoubleProperty("RightX", () -> getAxisForSendable(Axis.kRightX.value), null);
    builder.addDoubleProperty("RightY", () -> getAxisForSendable(Axis.kRightY.value), null);
    builder.addBooleanProperty(
        "SouthFace", () -> getButtonForSendable(Button.kSouthFace.value), null);
    builder.addBooleanProperty(
        "EastFace", () -> getButtonForSendable(Button.kEastFace.value), null);
    builder.addBooleanProperty(
        "WestFace", () -> getButtonForSendable(Button.kWestFace.value), null);
    builder.addBooleanProperty(
        "NorthFace", () -> getButtonForSendable(Button.kNorthFace.value), null);
    builder.addBooleanProperty("Back", () -> getButtonForSendable(Button.kBack.value), null);
    builder.addBooleanProperty("Guide", () -> getButtonForSendable(Button.kGuide.value), null);
    builder.addBooleanProperty("Start", () -> getButtonForSendable(Button.kStart.value), null);
    builder.addBooleanProperty(
        "LeftStick", () -> getButtonForSendable(Button.kLeftStick.value), null);
    builder.addBooleanProperty(
        "RightStick", () -> getButtonForSendable(Button.kRightStick.value), null);
    builder.addBooleanProperty(
        "LeftBumper", () -> getButtonForSendable(Button.kLeftBumper.value), null);
    builder.addBooleanProperty(
        "RightBumper", () -> getButtonForSendable(Button.kRightBumper.value), null);
    builder.addBooleanProperty("DpadUp", () -> getButtonForSendable(Button.kDpadUp.value), null);
    builder.addBooleanProperty(
        "DpadDown", () -> getButtonForSendable(Button.kDpadDown.value), null);
    builder.addBooleanProperty(
        "DpadLeft", () -> getButtonForSendable(Button.kDpadLeft.value), null);
    builder.addBooleanProperty(
        "DpadRight", () -> getButtonForSendable(Button.kDpadRight.value), null);
    builder.addBooleanProperty("Misc1", () -> getButtonForSendable(Button.kMisc1.value), null);
    builder.addBooleanProperty(
        "RightPaddle1", () -> getButtonForSendable(Button.kRightPaddle1.value), null);
    builder.addBooleanProperty(
        "LeftPaddle1", () -> getButtonForSendable(Button.kLeftPaddle1.value), null);
    builder.addBooleanProperty(
        "RightPaddle2", () -> getButtonForSendable(Button.kRightPaddle2.value), null);
    builder.addBooleanProperty(
        "LeftPaddle2", () -> getButtonForSendable(Button.kLeftPaddle2.value), null);
    builder.addBooleanProperty(
        "Touchpad", () -> getButtonForSendable(Button.kTouchpad.value), null);
    builder.addBooleanProperty("Misc2", () -> getButtonForSendable(Button.kMisc2.value), null);
    builder.addBooleanProperty("Misc3", () -> getButtonForSendable(Button.kMisc3.value), null);
    builder.addBooleanProperty("Misc4", () -> getButtonForSendable(Button.kMisc4.value), null);
    builder.addBooleanProperty("Misc5", () -> getButtonForSendable(Button.kMisc5.value), null);
    builder.addBooleanProperty("Misc6", () -> getButtonForSendable(Button.kMisc6.value), null);
  }
}
