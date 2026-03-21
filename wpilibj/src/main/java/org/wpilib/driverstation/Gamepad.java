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
    SOUTH_FACE(0, "SouthFaceButton"),
    /** East Face button. */
    EAST_FACE(1, "EastFaceButton"),
    /** West Face button. */
    WEST_FACE(2, "WestFaceButton"),
    /** North Face button. */
    NORTH_FACE(3, "NorthFaceButton"),
    /** Back button. */
    BACK(4, "BackButton"),
    /** Guide button. */
    GUIDE(5, "GuideButton"),
    /** Start button. */
    START(6, "StartButton"),
    /** Left stick button. */
    LEFT_STICK(7, "LeftStickButton"),
    /** Right stick button. */
    RIGHT_STICK(8, "RightStickButton"),
    /** Left bumper button. */
    LEFT_BUMPER(9, "LeftBumperButton"),
    /** Right bumper button. */
    RIGHT_BUMPER(10, "RightBumperButton"),
    /** D-pad up button. */
    DPAD_UP(11, "DpadUpButton"),
    /** D-pad down button. */
    DPAD_DOWN(12, "DpadDownButton"),
    /** D-pad left button. */
    DPAD_LEFT(13, "DpadLeftButton"),
    /** D-pad right button. */
    DPAD_RIGHT(14, "DpadRightButton"),
    /** Miscellaneous 1 button. */
    MISC_1(15, "Misc1Button"),
    /** Right Paddle 1 button. */
    RIGHT_PADDLE_1(16, "RightPaddle1Button"),
    /** Left Paddle 1 button. */
    LEFT_PADDLE_1(17, "LeftPaddle1Button"),
    /** Right Paddle 2 button. */
    RIGHT_PADDLE_2(18, "RightPaddle2Button"),
    /** Left Paddle 2 button. */
    LEFT_PADDLE_2(19, "LeftPaddle2Button"),
    /** Touchpad button. */
    TOUCHPAD(20, "TouchpadButton"),
    /** Miscellaneous 2 button. */
    MISC_2(21, "Misc2Button"),
    /** Miscellaneous 3 button. */
    MISC_3(22, "Misc3Button"),
    /** Miscellaneous 4 button. */
    MISC_4(23, "Misc4Button"),
    /** Miscellaneous 5 button. */
    MISC_5(24, "Misc5Button"),
    /** Miscellaneous 6 button. */
    MISC_6(25, "Misc6Button");

    /** Button value. */
    public final int value;

    private final String m_name;

    Button(int value, String name) {
      this.value = value;
      this.m_name = name;
    }

    /**
     * Get the human-friendly name of the button, matching the relevant methods.
     *
     * <p>Primarily used for automated unit tests.
     *
     * @return the human-friendly name of the button.
     */
    @Override
    public String toString() {
      return m_name;
    }
  }

  /** Represents an axis on an Gamepad. */
  public enum Axis {
    /** Left X axis. */
    LEFT_X(0, "LeftX"),
    /** Left Y axis. */
    LEFT_Y(1, "LeftY"),
    /** Right X axis. */
    RIGHT_X(2, "RightX"),
    /** Right Y axis. */
    RIGHT_Y(3, "RightY"),
    /** Left trigger. */
    LEFT_TRIGGER(4, "LeftTriggerAxis"),
    /** Right trigger. */
    RIGHT_TRIGGER(5, "RightTriggerAxis");

    /** Axis value. */
    public final int value;

    private final String m_name;

    Axis(int value, String name) {
      this.value = value;
      this.m_name = name;
    }

    /**
     * Get the human-friendly name of the axis, matching the relevant methods.
     *
     * <p>Primarily used for automated unit tests.
     *
     * @return the human-friendly name of the axis.
     */
    @Override
    public String toString() {
      return m_name;
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
    return getAxis(Axis.LEFT_X);
  }

  /**
   * Get the Y axis value of left side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return getAxis(Axis.LEFT_Y);
  }

  /**
   * Get the X axis value of right side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return getAxis(Axis.RIGHT_X);
  }

  /**
   * Get the Y axis value of right side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return getAxis(Axis.RIGHT_Y);
  }

  /**
   * Get the left trigger axis value of the controller. Note that this axis is bound to the range of
   * [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getLeftTriggerAxis() {
    return getAxis(Axis.LEFT_TRIGGER);
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
    return axisGreaterThan(Axis.LEFT_TRIGGER, threshold, loop);
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
    return getAxis(Axis.RIGHT_TRIGGER);
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
    return axisGreaterThan(Axis.RIGHT_TRIGGER, threshold, loop);
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
    return getButton(Button.SOUTH_FACE);
  }

  /**
   * Whether the South Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getSouthFaceButtonPressed() {
    return getButtonPressed(Button.SOUTH_FACE);
  }

  /**
   * Whether the South Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getSouthFaceButtonReleased() {
    return getButtonReleased(Button.SOUTH_FACE);
  }

  /**
   * Constructs an event instance around the South Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the South Face button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent southFace(EventLoop loop) {
    return button(Button.SOUTH_FACE, loop);
  }

  /**
   * Read the value of the East Face button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getEastFaceButton() {
    return getButton(Button.EAST_FACE);
  }

  /**
   * Whether the East Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getEastFaceButtonPressed() {
    return getButtonPressed(Button.EAST_FACE);
  }

  /**
   * Whether the East Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getEastFaceButtonReleased() {
    return getButtonReleased(Button.EAST_FACE);
  }

  /**
   * Constructs an event instance around the East Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the East Face button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent eastFace(EventLoop loop) {
    return button(Button.EAST_FACE, loop);
  }

  /**
   * Read the value of the West Face button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getWestFaceButton() {
    return getButton(Button.WEST_FACE);
  }

  /**
   * Whether the West Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getWestFaceButtonPressed() {
    return getButtonPressed(Button.WEST_FACE);
  }

  /**
   * Whether the West Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getWestFaceButtonReleased() {
    return getButtonReleased(Button.WEST_FACE);
  }

  /**
   * Constructs an event instance around the West Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the West Face button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent westFace(EventLoop loop) {
    return button(Button.WEST_FACE, loop);
  }

  /**
   * Read the value of the North Face button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getNorthFaceButton() {
    return getButton(Button.NORTH_FACE);
  }

  /**
   * Whether the North Face button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getNorthFaceButtonPressed() {
    return getButtonPressed(Button.NORTH_FACE);
  }

  /**
   * Whether the North Face button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getNorthFaceButtonReleased() {
    return getButtonReleased(Button.NORTH_FACE);
  }

  /**
   * Constructs an event instance around the North Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the North Face button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent northFace(EventLoop loop) {
    return button(Button.NORTH_FACE, loop);
  }

  /**
   * Read the value of the Back button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getBackButton() {
    return getButton(Button.BACK);
  }

  /**
   * Whether the Back button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getBackButtonPressed() {
    return getButtonPressed(Button.BACK);
  }

  /**
   * Whether the Back button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getBackButtonReleased() {
    return getButtonReleased(Button.BACK);
  }

  /**
   * Constructs an event instance around the Back button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Back button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent back(EventLoop loop) {
    return button(Button.BACK, loop);
  }

  /**
   * Read the value of the Guide button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getGuideButton() {
    return getButton(Button.GUIDE);
  }

  /**
   * Whether the Guide button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getGuideButtonPressed() {
    return getButtonPressed(Button.GUIDE);
  }

  /**
   * Whether the Guide button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getGuideButtonReleased() {
    return getButtonReleased(Button.GUIDE);
  }

  /**
   * Constructs an event instance around the Guide button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Guide button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent guide(EventLoop loop) {
    return button(Button.GUIDE, loop);
  }

  /**
   * Read the value of the Start button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getStartButton() {
    return getButton(Button.START);
  }

  /**
   * Whether the Start button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getStartButtonPressed() {
    return getButtonPressed(Button.START);
  }

  /**
   * Whether the Start button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getStartButtonReleased() {
    return getButtonReleased(Button.START);
  }

  /**
   * Constructs an event instance around the Start button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Start button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent start(EventLoop loop) {
    return button(Button.START, loop);
  }

  /**
   * Read the value of the left stick button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftStickButton() {
    return getButton(Button.LEFT_STICK);
  }

  /**
   * Whether the left stick button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftStickButtonPressed() {
    return getButtonPressed(Button.LEFT_STICK);
  }

  /**
   * Whether the left stick button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftStickButtonReleased() {
    return getButtonReleased(Button.LEFT_STICK);
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left stick button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent leftStick(EventLoop loop) {
    return button(Button.LEFT_STICK, loop);
  }

  /**
   * Read the value of the right stick button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightStickButton() {
    return getButton(Button.RIGHT_STICK);
  }

  /**
   * Whether the right stick button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightStickButtonPressed() {
    return getButtonPressed(Button.RIGHT_STICK);
  }

  /**
   * Whether the right stick button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightStickButtonReleased() {
    return getButtonReleased(Button.RIGHT_STICK);
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent rightStick(EventLoop loop) {
    return button(Button.RIGHT_STICK, loop);
  }

  /**
   * Read the value of the right bumper button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftBumperButton() {
    return getButton(Button.LEFT_BUMPER);
  }

  /**
   * Whether the right bumper button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftBumperButtonPressed() {
    return getButtonPressed(Button.LEFT_BUMPER);
  }

  /**
   * Whether the right bumper button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftBumperButtonReleased() {
    return getButtonReleased(Button.LEFT_BUMPER);
  }

  /**
   * Constructs an event instance around the right bumper button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent leftBumper(EventLoop loop) {
    return button(Button.LEFT_BUMPER, loop);
  }

  /**
   * Read the value of the right bumper button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightBumperButton() {
    return getButton(Button.RIGHT_BUMPER);
  }

  /**
   * Whether the right bumper button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightBumperButtonPressed() {
    return getButtonPressed(Button.RIGHT_BUMPER);
  }

  /**
   * Whether the right bumper button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightBumperButtonReleased() {
    return getButtonReleased(Button.RIGHT_BUMPER);
  }

  /**
   * Constructs an event instance around the right bumper button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent rightBumper(EventLoop loop) {
    return button(Button.RIGHT_BUMPER, loop);
  }

  /**
   * Read the value of the D-pad up button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getDpadUpButton() {
    return getButton(Button.DPAD_UP);
  }

  /**
   * Whether the D-pad up button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getDpadUpButtonPressed() {
    return getButtonPressed(Button.DPAD_UP);
  }

  /**
   * Whether the D-pad up button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getDpadUpButtonReleased() {
    return getButtonReleased(Button.DPAD_UP);
  }

  /**
   * Constructs an event instance around the D-pad up button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad up button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent dpadUp(EventLoop loop) {
    return button(Button.DPAD_UP, loop);
  }

  /**
   * Read the value of the D-pad down button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getDpadDownButton() {
    return getButton(Button.DPAD_DOWN);
  }

  /**
   * Whether the D-pad down button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getDpadDownButtonPressed() {
    return getButtonPressed(Button.DPAD_DOWN);
  }

  /**
   * Whether the D-pad down button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getDpadDownButtonReleased() {
    return getButtonReleased(Button.DPAD_DOWN);
  }

  /**
   * Constructs an event instance around the D-pad down button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad down button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent dpadDown(EventLoop loop) {
    return button(Button.DPAD_DOWN, loop);
  }

  /**
   * Read the value of the D-pad left button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getDpadLeftButton() {
    return getButton(Button.DPAD_LEFT);
  }

  /**
   * Whether the D-pad left button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getDpadLeftButtonPressed() {
    return getButtonPressed(Button.DPAD_LEFT);
  }

  /**
   * Whether the D-pad left button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getDpadLeftButtonReleased() {
    return getButtonReleased(Button.DPAD_LEFT);
  }

  /**
   * Constructs an event instance around the D-pad left button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad left button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent dpadLeft(EventLoop loop) {
    return button(Button.DPAD_LEFT, loop);
  }

  /**
   * Read the value of the D-pad right button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getDpadRightButton() {
    return getButton(Button.DPAD_RIGHT);
  }

  /**
   * Whether the D-pad right button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getDpadRightButtonPressed() {
    return getButtonPressed(Button.DPAD_RIGHT);
  }

  /**
   * Whether the D-pad right button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getDpadRightButtonReleased() {
    return getButtonReleased(Button.DPAD_RIGHT);
  }

  /**
   * Constructs an event instance around the D-pad right button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the D-pad right button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent dpadRight(EventLoop loop) {
    return button(Button.DPAD_RIGHT, loop);
  }

  /**
   * Read the value of the Miscellaneous 1 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc1Button() {
    return getButton(Button.MISC_1);
  }

  /**
   * Whether the Miscellaneous 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc1ButtonPressed() {
    return getButtonPressed(Button.MISC_1);
  }

  /**
   * Whether the Miscellaneous 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc1ButtonReleased() {
    return getButtonReleased(Button.MISC_1);
  }

  /**
   * Constructs an event instance around the Miscellaneous 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 1 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc1(EventLoop loop) {
    return button(Button.MISC_1, loop);
  }

  /**
   * Read the value of the Right Paddle 1 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightPaddle1Button() {
    return getButton(Button.RIGHT_PADDLE_1);
  }

  /**
   * Whether the Right Paddle 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightPaddle1ButtonPressed() {
    return getButtonPressed(Button.RIGHT_PADDLE_1);
  }

  /**
   * Whether the Right Paddle 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightPaddle1ButtonReleased() {
    return getButtonReleased(Button.RIGHT_PADDLE_1);
  }

  /**
   * Constructs an event instance around the Right Paddle 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Right Paddle 1 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent rightPaddle1(EventLoop loop) {
    return button(Button.RIGHT_PADDLE_1, loop);
  }

  /**
   * Read the value of the Left Paddle 1 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftPaddle1Button() {
    return getButton(Button.LEFT_PADDLE_1);
  }

  /**
   * Whether the Left Paddle 1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftPaddle1ButtonPressed() {
    return getButtonPressed(Button.LEFT_PADDLE_1);
  }

  /**
   * Whether the Left Paddle 1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftPaddle1ButtonReleased() {
    return getButtonReleased(Button.LEFT_PADDLE_1);
  }

  /**
   * Constructs an event instance around the Left Paddle 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Left Paddle 1 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent leftPaddle1(EventLoop loop) {
    return button(Button.LEFT_PADDLE_1, loop);
  }

  /**
   * Read the value of the Right Paddle 2 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightPaddle2Button() {
    return getButton(Button.RIGHT_PADDLE_2);
  }

  /**
   * Whether the Right Paddle 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightPaddle2ButtonPressed() {
    return getButtonPressed(Button.RIGHT_PADDLE_2);
  }

  /**
   * Whether the Right Paddle 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightPaddle2ButtonReleased() {
    return getButtonReleased(Button.RIGHT_PADDLE_2);
  }

  /**
   * Constructs an event instance around the Right Paddle 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Right Paddle 2 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent rightPaddle2(EventLoop loop) {
    return button(Button.RIGHT_PADDLE_2, loop);
  }

  /**
   * Read the value of the Left Paddle 2 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftPaddle2Button() {
    return getButton(Button.LEFT_PADDLE_2);
  }

  /**
   * Whether the Left Paddle 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftPaddle2ButtonPressed() {
    return getButtonPressed(Button.LEFT_PADDLE_2);
  }

  /**
   * Whether the Left Paddle 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftPaddle2ButtonReleased() {
    return getButtonReleased(Button.LEFT_PADDLE_2);
  }

  /**
   * Constructs an event instance around the Left Paddle 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Left Paddle 2 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent leftPaddle2(EventLoop loop) {
    return button(Button.LEFT_PADDLE_2, loop);
  }

  /**
   * Read the value of the Touchpad button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getTouchpadButton() {
    return getButton(Button.TOUCHPAD);
  }

  /**
   * Whether the Touchpad button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getTouchpadButtonPressed() {
    return getButtonPressed(Button.TOUCHPAD);
  }

  /**
   * Whether the Touchpad button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getTouchpadButtonReleased() {
    return getButtonReleased(Button.TOUCHPAD);
  }

  /**
   * Constructs an event instance around the Touchpad button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Touchpad button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent touchpad(EventLoop loop) {
    return button(Button.TOUCHPAD, loop);
  }

  /**
   * Read the value of the Miscellaneous 2 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc2Button() {
    return getButton(Button.MISC_2);
  }

  /**
   * Whether the Miscellaneous 2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc2ButtonPressed() {
    return getButtonPressed(Button.MISC_2);
  }

  /**
   * Whether the Miscellaneous 2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc2ButtonReleased() {
    return getButtonReleased(Button.MISC_2);
  }

  /**
   * Constructs an event instance around the Miscellaneous 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 2 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc2(EventLoop loop) {
    return button(Button.MISC_2, loop);
  }

  /**
   * Read the value of the Miscellaneous 3 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc3Button() {
    return getButton(Button.MISC_3);
  }

  /**
   * Whether the Miscellaneous 3 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc3ButtonPressed() {
    return getButtonPressed(Button.MISC_3);
  }

  /**
   * Whether the Miscellaneous 3 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc3ButtonReleased() {
    return getButtonReleased(Button.MISC_3);
  }

  /**
   * Constructs an event instance around the Miscellaneous 3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 3 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc3(EventLoop loop) {
    return button(Button.MISC_3, loop);
  }

  /**
   * Read the value of the Miscellaneous 4 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc4Button() {
    return getButton(Button.MISC_4);
  }

  /**
   * Whether the Miscellaneous 4 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc4ButtonPressed() {
    return getButtonPressed(Button.MISC_4);
  }

  /**
   * Whether the Miscellaneous 4 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc4ButtonReleased() {
    return getButtonReleased(Button.MISC_4);
  }

  /**
   * Constructs an event instance around the Miscellaneous 4 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 4 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc4(EventLoop loop) {
    return button(Button.MISC_4, loop);
  }

  /**
   * Read the value of the Miscellaneous 5 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc5Button() {
    return getButton(Button.MISC_5);
  }

  /**
   * Whether the Miscellaneous 5 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc5ButtonPressed() {
    return getButtonPressed(Button.MISC_5);
  }

  /**
   * Whether the Miscellaneous 5 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc5ButtonReleased() {
    return getButtonReleased(Button.MISC_5);
  }

  /**
   * Constructs an event instance around the Miscellaneous 5 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 5 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc5(EventLoop loop) {
    return button(Button.MISC_5, loop);
  }

  /**
   * Read the value of the Miscellaneous 6 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getMisc6Button() {
    return getButton(Button.MISC_6);
  }

  /**
   * Whether the Miscellaneous 6 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getMisc6ButtonPressed() {
    return getButtonPressed(Button.MISC_6);
  }

  /**
   * Whether the Miscellaneous 6 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getMisc6ButtonReleased() {
    return getButtonReleased(Button.MISC_6);
  }

  /**
   * Constructs an event instance around the Miscellaneous 6 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Miscellaneous 6 button's digital signal attached to
   *     the given loop.
   */
  public BooleanEvent misc6(EventLoop loop) {
    return button(Button.MISC_6, loop);
  }

  /**
   * Get the button value (starting at button 1).
   *
   * <p>This method returns true if the button is being held down at the time that this method is
   * being called.
   *
   * @param button The button
   * @return The state of the button.
   */
  public boolean getButton(Button button) {
    return getRawButton(button.value);
  }

  /**
   * Whether the button was pressed since the last check.
   *
   * <p>This method returns true if the button went from not pressed to held down since the last
   * time this method was called. This is useful if you only want to call a function once when you
   * press the button.
   *
   * @param button The button
   * @return Whether the button was pressed since the last check.
   */
  public boolean getButtonPressed(Button button) {
    return getRawButtonPressed(button.value);
  }

  /**
   * Whether the button was released since the last check.
   *
   * <p>This method returns true if the button went from held down to not pressed since the last
   * time this method was called. This is useful if you only want to call a function once when you
   * release the button.
   *
   * @param button The button
   * @return Whether the button was released since the last check.
   */
  public boolean getButtonReleased(Button button) {
    return getRawButtonReleased(button.value);
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached to the given loop.
   */
  public BooleanEvent button(Button button, EventLoop loop) {
    return super.button(button.value, loop);
  }

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read
   * @return The value of the axis.
   */
  public double getAxis(Axis axis) {
    return getRawAxis(axis.value);
  }

  /**
   * Constructs an event instance that is true when the axis value is less than {@code threshold},
   * attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value below which this event should return true.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the axis value is less than the provided threshold.
   */
  public BooleanEvent axisLessThan(Axis axis, double threshold, EventLoop loop) {
    return super.axisLessThan(axis.value, threshold, loop);
  }

  /**
   * Constructs an event instance that is true when the axis value is greater than {@code
   * threshold}, attached to the given loop.
   *
   * @param axis The axis to read, starting at 0
   * @param threshold The value above which this event should return true.
   * @param loop the event loop instance to attach the event to.
   * @return an event instance that is true when the axis value is greater than the provided
   *     threshold.
   */
  public BooleanEvent axisGreaterThan(Axis axis, double threshold, EventLoop loop) {
    return super.axisGreaterThan(axis.value, threshold, loop);
  }

  private double getAxisForSendable(Axis axis) {
    return DriverStation.getStickAxisIfAvailable(getPort(), axis.value).orElse(0.0);
  }

  private boolean getButtonForSendable(Button button) {
    return DriverStation.getStickButtonIfAvailable(getPort(), button.value).orElse(false);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("HID");
    builder.publishConstString("ControllerType", "Gamepad");
    builder.addDoubleProperty(
        "LeftTrigger Axis", () -> getAxisForSendable(Axis.LEFT_TRIGGER), null);
    builder.addDoubleProperty(
        "RightTrigger Axis", () -> getAxisForSendable(Axis.RIGHT_TRIGGER), null);
    builder.addDoubleProperty("LeftX", () -> getAxisForSendable(Axis.LEFT_X), null);
    builder.addDoubleProperty("LeftY", () -> getAxisForSendable(Axis.LEFT_Y), null);
    builder.addDoubleProperty("RightX", () -> getAxisForSendable(Axis.RIGHT_X), null);
    builder.addDoubleProperty("RightY", () -> getAxisForSendable(Axis.RIGHT_Y), null);
    builder.addBooleanProperty("SouthFace", () -> getButtonForSendable(Button.SOUTH_FACE), null);
    builder.addBooleanProperty("EastFace", () -> getButtonForSendable(Button.EAST_FACE), null);
    builder.addBooleanProperty("WestFace", () -> getButtonForSendable(Button.WEST_FACE), null);
    builder.addBooleanProperty("NorthFace", () -> getButtonForSendable(Button.NORTH_FACE), null);
    builder.addBooleanProperty("Back", () -> getButtonForSendable(Button.BACK), null);
    builder.addBooleanProperty("Guide", () -> getButtonForSendable(Button.GUIDE), null);
    builder.addBooleanProperty("Start", () -> getButtonForSendable(Button.START), null);
    builder.addBooleanProperty("LeftStick", () -> getButtonForSendable(Button.LEFT_STICK), null);
    builder.addBooleanProperty("RightStick", () -> getButtonForSendable(Button.RIGHT_STICK), null);
    builder.addBooleanProperty("LeftBumper", () -> getButtonForSendable(Button.LEFT_BUMPER), null);
    builder.addBooleanProperty(
        "RightBumper", () -> getButtonForSendable(Button.RIGHT_BUMPER), null);
    builder.addBooleanProperty("DpadUp", () -> getButtonForSendable(Button.DPAD_UP), null);
    builder.addBooleanProperty("DpadDown", () -> getButtonForSendable(Button.DPAD_DOWN), null);
    builder.addBooleanProperty("DpadLeft", () -> getButtonForSendable(Button.DPAD_LEFT), null);
    builder.addBooleanProperty("DpadRight", () -> getButtonForSendable(Button.DPAD_RIGHT), null);
    builder.addBooleanProperty("Misc1", () -> getButtonForSendable(Button.MISC_1), null);
    builder.addBooleanProperty(
        "RightPaddle1", () -> getButtonForSendable(Button.RIGHT_PADDLE_1), null);
    builder.addBooleanProperty(
        "LeftPaddle1", () -> getButtonForSendable(Button.LEFT_PADDLE_1), null);
    builder.addBooleanProperty(
        "RightPaddle2", () -> getButtonForSendable(Button.RIGHT_PADDLE_2), null);
    builder.addBooleanProperty(
        "LeftPaddle2", () -> getButtonForSendable(Button.LEFT_PADDLE_2), null);
    builder.addBooleanProperty("Touchpad", () -> getButtonForSendable(Button.TOUCHPAD), null);
    builder.addBooleanProperty("Misc2", () -> getButtonForSendable(Button.MISC_2), null);
    builder.addBooleanProperty("Misc3", () -> getButtonForSendable(Button.MISC_3), null);
    builder.addBooleanProperty("Misc4", () -> getButtonForSendable(Button.MISC_4), null);
    builder.addBooleanProperty("Misc5", () -> getButtonForSendable(Button.MISC_5), null);
    builder.addBooleanProperty("Misc6", () -> getButtonForSendable(Button.MISC_6), null);
  }
}
