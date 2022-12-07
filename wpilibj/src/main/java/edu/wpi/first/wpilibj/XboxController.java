// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.event.BooleanEvent;
import edu.wpi.first.wpilibj.event.EventLoop;

/**
 * Handle input from Xbox 360 or Xbox One controllers connected to the Driver Station.
 *
 * <p>This class handles Xbox input that comes from the Driver Station. Each time a value is
 * requested the most recent value is returned. There is a single class instance for each controller
 * and the mapping of ports to hardware buttons depends on the code in the Driver Station.
 */
public class XboxController extends GenericHID {
  /** Represents a digital button on an XboxController. */
  public enum Button {
    kLeftBumper(5),
    kRightBumper(6),
    kLeftStick(9),
    kRightStick(10),
    kA(1),
    kB(2),
    kX(3),
    kY(4),
    kBack(7),
    kStart(8);

    public final int value;

    Button(int value) {
      this.value = value;
    }

    /**
     * Get the human-friendly name of the button, matching the relevant methods. This is done by
     * stripping the leading `k`, and if not a Bumper button append `Button`.
     *
     * <p>Primarily used for automated unit tests.
     *
     * @return the human-friendly name of the button.
     */
    @Override
    public String toString() {
      var name = this.name().substring(1); // Remove leading `k`
      if (name.endsWith("Bumper")) {
        return name;
      }
      return name + "Button";
    }
  }

  /** Represents an axis on an XboxController. */
  public enum Axis {
    kLeftX(0),
    kRightX(4),
    kLeftY(1),
    kRightY(5),
    kLeftTrigger(2),
    kRightTrigger(3);

    public final int value;

    Axis(int value) {
      this.value = value;
    }

    /**
     * Get the human-friendly name of the axis, matching the relevant methods. This is done by
     * stripping the leading `k`, and if a trigger axis append `Axis`.
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
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public XboxController(final int port) {
    super(port);

    HAL.report(tResourceType.kResourceType_XboxController, port + 1);
  }

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftX() {
    return getRawAxis(Axis.kLeftX.value);
  }

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return getRawAxis(Axis.kRightX.value);
  }

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return getRawAxis(Axis.kLeftY.value);
  }

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return getRawAxis(Axis.kRightY.value);
  }

  /**
   * Get the left trigger (LT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getLeftTriggerAxis() {
    return getRawAxis(Axis.kLeftTrigger.value);
  }

  /**
   * Get the right trigger (RT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getRightTriggerAxis() {
    return getRawAxis(Axis.kRightTrigger.value);
  }

  /**
   * Read the value of the left bumper (LB) button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftBumper() {
    return getRawButton(Button.kLeftBumper.value);
  }

  /**
   * Read the value of the right bumper (RB) button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightBumper() {
    return getRawButton(Button.kRightBumper.value);
  }

  /**
   * Whether the left bumper (LB) was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftBumperPressed() {
    return getRawButtonPressed(Button.kLeftBumper.value);
  }

  /**
   * Whether the right bumper (RB) was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightBumperPressed() {
    return getRawButtonPressed(Button.kRightBumper.value);
  }

  /**
   * Whether the left bumper (LB) was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftBumperReleased() {
    return getRawButtonReleased(Button.kLeftBumper.value);
  }

  /**
   * Whether the right bumper (RB) was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightBumperReleased() {
    return getRawButtonReleased(Button.kRightBumper.value);
  }

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent leftBumper(EventLoop loop) {
    return new BooleanEvent(loop, this::getLeftBumper);
  }

  /**
   * Constructs an event instance around the left bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left bumper's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent rightBumper(EventLoop loop) {
    return new BooleanEvent(loop, this::getRightBumper);
  }

  /**
   * Read the value of the left stick button (LSB) on the controller.
   *
   * @return The state of the button.
   */
  public boolean getLeftStickButton() {
    return getRawButton(Button.kLeftStick.value);
  }

  /**
   * Read the value of the right stick button (RSB) on the controller.
   *
   * @return The state of the button.
   */
  public boolean getRightStickButton() {
    return getRawButton(Button.kRightStick.value);
  }

  /**
   * Whether the left stick button (LSB) was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getLeftStickButtonPressed() {
    return getRawButtonPressed(Button.kLeftStick.value);
  }

  /**
   * Whether the right stick button (RSB) was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getRightStickButtonPressed() {
    return getRawButtonPressed(Button.kRightStick.value);
  }

  /**
   * Whether the left stick button (LSB) was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getLeftStickButtonReleased() {
    return getRawButtonReleased(Button.kLeftStick.value);
  }

  /**
   * Whether the right stick (RSB) button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getRightStickButtonReleased() {
    return getRawButtonReleased(Button.kRightStick.value);
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left stick button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent leftStick(EventLoop loop) {
    return new BooleanEvent(loop, this::getLeftStickButton);
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public BooleanEvent rightStick(EventLoop loop) {
    return new BooleanEvent(loop, this::getRightStickButton);
  }

  /**
   * Read the value of the A button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getAButton() {
    return getRawButton(Button.kA.value);
  }

  /**
   * Whether the A button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getAButtonPressed() {
    return getRawButtonPressed(Button.kA.value);
  }

  /**
   * Whether the A button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getAButtonReleased() {
    return getRawButtonReleased(Button.kA.value);
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the A button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent a(EventLoop loop) {
    return new BooleanEvent(loop, this::getAButton);
  }

  /**
   * Read the value of the B button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getBButton() {
    return getRawButton(Button.kB.value);
  }

  /**
   * Whether the B button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getBButtonPressed() {
    return getRawButtonPressed(Button.kB.value);
  }

  /**
   * Whether the B button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getBButtonReleased() {
    return getRawButtonReleased(Button.kB.value);
  }

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the B button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent b(EventLoop loop) {
    return new BooleanEvent(loop, this::getBButton);
  }

  /**
   * Read the value of the X button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getXButton() {
    return getRawButton(Button.kX.value);
  }

  /**
   * Whether the X button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getXButtonPressed() {
    return getRawButtonPressed(Button.kX.value);
  }

  /**
   * Whether the X button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getXButtonReleased() {
    return getRawButtonReleased(Button.kX.value);
  }

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the X button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent x(EventLoop loop) {
    return new BooleanEvent(loop, this::getXButton);
  }

  /**
   * Read the value of the Y button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getYButton() {
    return getRawButton(Button.kY.value);
  }

  /**
   * Whether the Y button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getYButtonPressed() {
    return getRawButtonPressed(Button.kY.value);
  }

  /**
   * Whether the Y button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getYButtonReleased() {
    return getRawButtonReleased(Button.kY.value);
  }

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Y button's digital signal attached to the given
   *     loop.
   */
  @SuppressWarnings("MethodName")
  public BooleanEvent y(EventLoop loop) {
    return new BooleanEvent(loop, this::getYButton);
  }

  /**
   * Read the value of the back button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getBackButton() {
    return getRawButton(Button.kBack.value);
  }

  /**
   * Whether the back button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getBackButtonPressed() {
    return getRawButtonPressed(Button.kBack.value);
  }

  /**
   * Whether the back button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getBackButtonReleased() {
    return getRawButtonReleased(Button.kBack.value);
  }

  /**
   * Constructs an event instance around the back button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the back button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent back(EventLoop loop) {
    return new BooleanEvent(loop, this::getBackButton);
  }

  /**
   * Read the value of the start button on the controller.
   *
   * @return The state of the button.
   */
  public boolean getStartButton() {
    return getRawButton(Button.kStart.value);
  }

  /**
   * Whether the start button was pressed since the last check.
   *
   * @return Whether the button was pressed since the last check.
   */
  public boolean getStartButtonPressed() {
    return getRawButtonPressed(Button.kStart.value);
  }

  /**
   * Whether the start button was released since the last check.
   *
   * @return Whether the button was released since the last check.
   */
  public boolean getStartButtonReleased() {
    return getRawButtonReleased(Button.kStart.value);
  }

  /**
   * Constructs an event instance around the start button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the start button's digital signal attached to the given
   *     loop.
   */
  public BooleanEvent start(EventLoop loop) {
    return new BooleanEvent(loop, this::getStartButton);
  }
}
