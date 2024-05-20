// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.event.BooleanEvent;
import edu.wpi.first.wpilibj.event.EventLoop;

/**
 * Handle input from PS5 controllers connected to the Driver Station.
 *
 * <p>This class handles PS5 input that comes from the Driver Station. Each time a value is
 * requested the most recent value is returned. There is a single class instance for each controller
 * and the mapping of ports to hardware buttons depends on the code in the Driver Station.
 *
 * <p>Only first party controllers from Sony are guaranteed to have the correct mapping, and only
 * through the official NI DS. Sim is not guaranteed to have the same mapping, as well as any 3rd
 * party controllers.
 */
public class PS5Controller extends GenericHID {
  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public PS5Controller(int port) {
    super(port);
    // HAL.report(tResourceType.kResourceType_PS5Controller, port + 1);
  }

  /** Represents a digital button on a PS5Controller. */
  public enum Button {
    /** Square button. */
    kSquare(1),
    /** X button. */
    kCross(2),
    /** Circle button. */
    kCircle(3),
    /** Triangle button. */
    kTriangle(4),
    /** Left trigger 1 button. */
    kL1(5),
    /** Right trigger 1 button. */
    kR1(6),
    /** Left trigger 2 button. */
    kL2(7),
    /** Right trigger 2 button. */
    kR2(8),
    /** Create button. */
    kCreate(9),
    /** Options button. */
    kOptions(10),
    /** Left stick button. */
    kL3(11),
    /** Right stick button. */
    kR3(12),
    /** PlayStation button. */
    kPS(13),
    /** Touchpad click button. */
    kTouchpad(14);

    /** Button value. */
    public final int value;

    Button(int index) {
      this.value = index;
    }

    /**
     * Get the human-friendly name of the button, matching the relevant methods. This is done by
     * stripping the leading `k`, and if not the touchpad append `Button`.
     *
     * <p>Primarily used for automated unit tests.
     *
     * @return the human-friendly name of the button.
     */
    @Override
    public String toString() {
      var name = this.name().substring(1); // Remove leading `k`
      if (this == kTouchpad) {
        return name;
      }
      return name + "Button";
    }
  }

  /** Represents an axis on a PS5Controller. */
  public enum Axis {
    /** Left X axis. */
    kLeftX(0),
    /** Left Y axis. */
    kLeftY(1),
    /** Right X axis. */
    kRightX(2),
    /** Right Y axis. */
    kRightY(5),
    /** Left Trigger 2. */
    kL2(3),
    /** Right Trigger 2. */
    kR2(4);

    /** Axis value. */
    public final int value;

    Axis(int index) {
      value = index;
    }

    /**
     * Get the human-friendly name of the axis, matching the relevant methods. This is done by
     * stripping the leading `k`, and if one of L2/R2 append `Axis`.
     *
     * <p>Primarily used for automated unit tests.
     *
     * @return the human-friendly name of the axis.
     */
    @Override
    public String toString() {
      var name = this.name().substring(1); // Remove leading `k`
      if (name.endsWith("2")) {
        return name + "Axis";
      }
      return name;
    }
  }

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return the axis value.
   */
  public double getLeftX() {
    return getRawAxis(Axis.kLeftX.value);
  }

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return the axis value.
   */
  public double getRightX() {
    return getRawAxis(Axis.kRightX.value);
  }

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return the axis value.
   */
  public double getLeftY() {
    return getRawAxis(Axis.kLeftY.value);
  }

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return the axis value.
   */
  public double getRightY() {
    return getRawAxis(Axis.kRightY.value);
  }

  /**
   * Get the L2 axis value of the controller. Note that this axis is bound to the range of [0, 1] as
   * opposed to the usual [-1, 1].
   *
   * @return the axis value.
   */
  public double getL2Axis() {
    return getRawAxis(Axis.kL2.value);
  }

  /**
   * Get the R2 axis value of the controller. Note that this axis is bound to the range of [0, 1] as
   * opposed to the usual [-1, 1].
   *
   * @return the axis value.
   */
  public double getR2Axis() {
    return getRawAxis(Axis.kR2.value);
  }

  /**
   * Read the value of the left trigger button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getL2Button() {
    return getRawButton(Button.kL2.value);
  }

  /**
   * Read the value of the right trigger button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getR2Button() {
    return getRawButton(Button.kR2.value);
  }

  /**
   * Whether the L2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getL2ButtonPressed() {
    return getRawButtonPressed(Button.kL2.value);
  }

  /**
   * Whether the R2 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getR2ButtonPressed() {
    return getRawButtonPressed(Button.kR2.value);
  }

  /**
   * Whether the L2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getL2ButtonReleased() {
    return getRawButtonReleased(Button.kL2.value);
  }

  /**
   * Whether the R2 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getR2ButtonReleased() {
    return getRawButtonReleased(Button.kR2.value);
  }

  /**
   * Constructs an event instance around the L2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L2 button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent L2(EventLoop loop) {
    return new BooleanEvent(loop, this::getL2Button);
  }

  /**
   * Constructs an event instance around the R2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R2 button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent R2(EventLoop loop) {
    return new BooleanEvent(loop, this::getR2Button);
  }

  /**
   * Read the value of the L1 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getL1Button() {
    return getRawButton(Button.kL1.value);
  }

  /**
   * Read the value of the R1 button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getR1Button() {
    return getRawButton(Button.kR1.value);
  }

  /**
   * Whether the L1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getL1ButtonPressed() {
    return getRawButtonPressed(Button.kL1.value);
  }

  /**
   * Whether the R1 button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getR1ButtonPressed() {
    return getRawButtonPressed(Button.kR1.value);
  }

  /**
   * Whether the L1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getL1ButtonReleased() {
    return getRawButtonReleased(Button.kL1.value);
  }

  /**
   * Whether the R1 button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getR1ButtonReleased() {
    return getRawButtonReleased(Button.kR1.value);
  }

  /**
   * Constructs an event instance around the L1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L1 button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent L1(EventLoop loop) {
    return new BooleanEvent(loop, this::getL1Button);
  }

  /**
   * Constructs an event instance around the R1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R1 button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent R1(EventLoop loop) {
    return new BooleanEvent(loop, this::getR1Button);
  }

  /**
   * Read the value of the L3 button (pressing the left analog stick) on the controller.
   *
   * @return The state of the button.
   */
  public boolean getL3Button() {
    return getRawButton(Button.kL3.value);
  }

  /**
   * Read the value of the R3 button (pressing the right analog stick) on the controller.
   *
   * @return The state of the button.
   */
  public boolean getR3Button() {
    return getRawButton(Button.kR3.value);
  }

  /**
   * Whether the L3 (left stick) button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getL3ButtonPressed() {
    return getRawButtonPressed(Button.kL3.value);
  }

  /**
   * Whether the R3 (right stick) button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getR3ButtonPressed() {
    return getRawButtonPressed(Button.kR3.value);
  }

  /**
   * Whether the L3 (left stick) button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getL3ButtonReleased() {
    return getRawButtonReleased(Button.kL3.value);
  }

  /**
   * Whether the R3 (right stick) button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getR3ButtonReleased() {
    return getRawButtonReleased(Button.kR3.value);
  }

  /**
   * Constructs an event instance around the L3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L3 button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent L3(EventLoop loop) {
    return new BooleanEvent(loop, this::getL3Button);
  }

  /**
   * Constructs an event instance around the R3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R3 button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent R3(EventLoop loop) {
    return new BooleanEvent(loop, this::getR3Button);
  }

  /**
   * Read the value of the Square button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getSquareButton() {
    return getRawButton(Button.kSquare.value);
  }

  /**
   * Whether the Square button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getSquareButtonPressed() {
    return getRawButtonPressed(Button.kSquare.value);
  }

  /**
   * Whether the Square button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getSquareButtonReleased() {
    return getRawButtonReleased(Button.kSquare.value);
  }

  /**
   * Constructs an event instance around the square button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the square button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent square(EventLoop loop) {
    return new BooleanEvent(loop, this::getSquareButton);
  }

  /**
   * Read the value of the Cross button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getCrossButton() {
    return getRawButton(Button.kCross.value);
  }

  /**
   * Whether the Cross button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getCrossButtonPressed() {
    return getRawButtonPressed(Button.kCross.value);
  }

  /**
   * Whether the Cross button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getCrossButtonReleased() {
    return getRawButtonReleased(Button.kCross.value);
  }

  /**
   * Constructs an event instance around the cross button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the cross button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent cross(EventLoop loop) {
    return new BooleanEvent(loop, this::getCrossButton);
  }

  /**
   * Read the value of the Triangle button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getTriangleButton() {
    return getRawButton(Button.kTriangle.value);
  }

  /**
   * Whether the Triangle button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getTriangleButtonPressed() {
    return getRawButtonPressed(Button.kTriangle.value);
  }

  /**
   * Whether the Triangle button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getTriangleButtonReleased() {
    return getRawButtonReleased(Button.kTriangle.value);
  }

  /**
   * Constructs an event instance around the triangle button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the triangle button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent triangle(EventLoop loop) {
    return new BooleanEvent(loop, this::getTriangleButton);
  }

  /**
   * Read the value of the Circle button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getCircleButton() {
    return getRawButton(Button.kCircle.value);
  }

  /**
   * Whether the Circle button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getCircleButtonPressed() {
    return getRawButtonPressed(Button.kCircle.value);
  }

  /**
   * Whether the Circle button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getCircleButtonReleased() {
    return getRawButtonReleased(Button.kCircle.value);
  }

  /**
   * Constructs an event instance around the circle button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the circle button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent circle(EventLoop loop) {
    return new BooleanEvent(loop, this::getCircleButton);
  }

  /**
   * Read the value of the share button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getCreateButton() {
    return getRawButton(Button.kCreate.value);
  }

  /**
   * Whether the share button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getCreateButtonPressed() {
    return getRawButtonPressed(Button.kCreate.value);
  }

  /**
   * Whether the share button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getCreateButtonReleased() {
    return getRawButtonReleased(Button.kCreate.value);
  }

  /**
   * Constructs an event instance around the share button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the share button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent create(EventLoop loop) {
    return new BooleanEvent(loop, this::getCreateButton);
  }

  /**
   * Read the value of the PS button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getPSButton() {
    return getRawButton(Button.kPS.value);
  }

  /**
   * Whether the PS button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getPSButtonPressed() {
    return getRawButtonPressed(Button.kPS.value);
  }

  /**
   * Whether the PS button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getPSButtonReleased() {
    return getRawButtonReleased(Button.kPS.value);
  }

  /**
   * Constructs an event instance around the PS button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the PS button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent PS(EventLoop loop) {
    return new BooleanEvent(loop, this::getPSButton);
  }

  /**
   * Read the value of the options button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getOptionsButton() {
    return getRawButton(Button.kOptions.value);
  }

  /**
   * Whether the options button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getOptionsButtonPressed() {
    return getRawButtonPressed(Button.kOptions.value);
  }

  /**
   * Whether the options button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getOptionsButtonReleased() {
    return getRawButtonReleased(Button.kOptions.value);
  }

  /**
   * Constructs an event instance around the options button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the options button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent options(EventLoop loop) {
    return new BooleanEvent(loop, this::getOptionsButton);
  }

  /**
   * Read the value of the touchpad on the controller.
   *
   * @return The state of the touchpad.
   */
  public boolean getTouchpad() {
    return getRawButton(Button.kTouchpad.value);
  }

  /**
   * Whether the touchpad was pressed since the last check.
   *
   * @return Whether the touchpad was pressed since the last check.
   */
  public boolean getTouchpadPressed() {
    return getRawButtonPressed(Button.kTouchpad.value);
  }

  /**
   * Whether the touchpad was released since the last check.
   *
   * @return Whether the touchpad was released since the last check.
   */
  public boolean getTouchpadReleased() {
    return getRawButtonReleased(Button.kTouchpad.value);
  }

  /**
   * Constructs an event instance around the touchpad's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the touchpad's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent touchpad(EventLoop loop) {
    return new BooleanEvent(loop, this::getTouchpad);
  }
}
