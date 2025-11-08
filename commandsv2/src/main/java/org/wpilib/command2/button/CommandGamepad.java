// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2.button;

import org.wpilib.driverstation.Gamepad;
import org.wpilib.event.EventLoop;
import org.wpilib.command2.CommandScheduler;

/**
 * A version of {@link Gamepad} with {@link Trigger} factories for command-based.
 *
 * @see Gamepad
 */
@SuppressWarnings("MethodName")
public class CommandGamepad extends CommandGenericHID {
  private final Gamepad m_hid;

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandGamepad(int port) {
    super(port);
    m_hid = new Gamepad(port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  @Override
  public Gamepad getHID() {
    return m_hid;
  }

  /**
   * Constructs a Trigger instance around the South Face button's digital signal.
   *
   * @return a Trigger instance representing the South Face button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #southFace(EventLoop)
   */
  public Trigger southFace() {
    return southFace(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the South Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the South Face button's digital signal attached to the
   *     given loop.
   */
  public Trigger southFace(EventLoop loop) {
    return button(Gamepad.Button.kSouthFace.value, loop);
  }

  /**
   * Constructs a Trigger instance around the East Face button's digital signal.
   *
   * @return a Trigger instance representing the East Face button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #eastFace(EventLoop)
   */
  public Trigger eastFace() {
    return eastFace(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the East Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the East Face button's digital signal attached to the
   *     given loop.
   */
  public Trigger eastFace(EventLoop loop) {
    return button(Gamepad.Button.kEastFace.value, loop);
  }

  /**
   * Constructs a Trigger instance around the West Face button's digital signal.
   *
   * @return a Trigger instance representing the West Face button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #westFace(EventLoop)
   */
  public Trigger westFace() {
    return westFace(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the West Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the West Face button's digital signal attached to the
   *     given loop.
   */
  public Trigger westFace(EventLoop loop) {
    return button(Gamepad.Button.kWestFace.value, loop);
  }

  /**
   * Constructs a Trigger instance around the North Face button's digital signal.
   *
   * @return a Trigger instance representing the North Face button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #northFace(EventLoop)
   */
  public Trigger northFace() {
    return northFace(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the North Face button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the North Face button's digital signal attached to the
   *     given loop.
   */
  public Trigger northFace(EventLoop loop) {
    return button(Gamepad.Button.kNorthFace.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Back button's digital signal.
   *
   * @return a Trigger instance representing the Back button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #back(EventLoop)
   */
  public Trigger back() {
    return back(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Back button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Back button's digital signal attached to the given
   *     loop.
   */
  public Trigger back(EventLoop loop) {
    return button(Gamepad.Button.kBack.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Guide button's digital signal.
   *
   * @return a Trigger instance representing the Guide button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #guide(EventLoop)
   */
  public Trigger guide() {
    return guide(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Guide button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Guide button's digital signal attached to the given
   *     loop.
   */
  public Trigger guide(EventLoop loop) {
    return button(Gamepad.Button.kGuide.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Start button's digital signal.
   *
   * @return a Trigger instance representing the Start button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #start(EventLoop)
   */
  public Trigger start() {
    return start(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Start button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Start button's digital signal attached to the given
   *     loop.
   */
  public Trigger start(EventLoop loop) {
    return button(Gamepad.Button.kStart.value, loop);
  }

  /**
   * Constructs a Trigger instance around the left stick button's digital signal.
   *
   * @return a Trigger instance representing the left stick button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftStick(EventLoop)
   */
  public Trigger leftStick() {
    return leftStick(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the left stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the left stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger leftStick(EventLoop loop) {
    return button(Gamepad.Button.kLeftStick.value, loop);
  }

  /**
   * Constructs a Trigger instance around the right stick button's digital signal.
   *
   * @return a Trigger instance representing the right stick button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightStick(EventLoop)
   */
  public Trigger rightStick() {
    return rightStick(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the right stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger rightStick(EventLoop loop) {
    return button(Gamepad.Button.kRightStick.value, loop);
  }

  /**
   * Constructs a Trigger instance around the right shoulder button's digital signal.
   *
   * @return a Trigger instance representing the right shoulder button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftShoulder(EventLoop)
   */
  public Trigger leftShoulder() {
    return leftShoulder(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the right shoulder button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the right shoulder button's digital signal attached to
   *     the given loop.
   */
  public Trigger leftShoulder(EventLoop loop) {
    return button(Gamepad.Button.kLeftShoulder.value, loop);
  }

  /**
   * Constructs a Trigger instance around the right shoulder button's digital signal.
   *
   * @return a Trigger instance representing the right shoulder button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightShoulder(EventLoop)
   */
  public Trigger rightShoulder() {
    return rightShoulder(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the right shoulder button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the right shoulder button's digital signal attached to
   *     the given loop.
   */
  public Trigger rightShoulder(EventLoop loop) {
    return button(Gamepad.Button.kRightShoulder.value, loop);
  }

  /**
   * Constructs a Trigger instance around the D-pad up button's digital signal.
   *
   * @return a Trigger instance representing the D-pad up button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #dpadUp(EventLoop)
   */
  public Trigger dpadUp() {
    return dpadUp(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the D-pad up button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the D-pad up button's digital signal attached to the
   *     given loop.
   */
  public Trigger dpadUp(EventLoop loop) {
    return button(Gamepad.Button.kDpadUp.value, loop);
  }

  /**
   * Constructs a Trigger instance around the D-pad down button's digital signal.
   *
   * @return a Trigger instance representing the D-pad down button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #dpadDown(EventLoop)
   */
  public Trigger dpadDown() {
    return dpadDown(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the D-pad down button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the D-pad down button's digital signal attached to the
   *     given loop.
   */
  public Trigger dpadDown(EventLoop loop) {
    return button(Gamepad.Button.kDpadDown.value, loop);
  }

  /**
   * Constructs a Trigger instance around the D-pad left button's digital signal.
   *
   * @return a Trigger instance representing the D-pad left button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #dpadLeft(EventLoop)
   */
  public Trigger dpadLeft() {
    return dpadLeft(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the D-pad left button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the D-pad left button's digital signal attached to the
   *     given loop.
   */
  public Trigger dpadLeft(EventLoop loop) {
    return button(Gamepad.Button.kDpadLeft.value, loop);
  }

  /**
   * Constructs a Trigger instance around the D-pad right button's digital signal.
   *
   * @return a Trigger instance representing the D-pad right button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #dpadRight(EventLoop)
   */
  public Trigger dpadRight() {
    return dpadRight(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the D-pad right button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the D-pad right button's digital signal attached to the
   *     given loop.
   */
  public Trigger dpadRight(EventLoop loop) {
    return button(Gamepad.Button.kDpadRight.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 1 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 1 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #misc1(EventLoop)
   */
  public Trigger misc1() {
    return misc1(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 1 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc1(EventLoop loop) {
    return button(Gamepad.Button.kMisc1.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Right Paddle 1 button's digital signal.
   *
   * @return a Trigger instance representing the Right Paddle 1 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightPaddle1(EventLoop)
   */
  public Trigger rightPaddle1() {
    return rightPaddle1(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Right Paddle 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Right Paddle 1 button's digital signal attached to
   *     the given loop.
   */
  public Trigger rightPaddle1(EventLoop loop) {
    return button(Gamepad.Button.kRightPaddle1.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Left Paddle 1 button's digital signal.
   *
   * @return a Trigger instance representing the Left Paddle 1 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftPaddle1(EventLoop)
   */
  public Trigger leftPaddle1() {
    return leftPaddle1(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Left Paddle 1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Left Paddle 1 button's digital signal attached to
   *     the given loop.
   */
  public Trigger leftPaddle1(EventLoop loop) {
    return button(Gamepad.Button.kLeftPaddle1.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Right Paddle 2 button's digital signal.
   *
   * @return a Trigger instance representing the Right Paddle 2 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightPaddle2(EventLoop)
   */
  public Trigger rightPaddle2() {
    return rightPaddle2(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Right Paddle 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Right Paddle 2 button's digital signal attached to
   *     the given loop.
   */
  public Trigger rightPaddle2(EventLoop loop) {
    return button(Gamepad.Button.kRightPaddle2.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Left Paddle 2 button's digital signal.
   *
   * @return a Trigger instance representing the Left Paddle 2 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftPaddle2(EventLoop)
   */
  public Trigger leftPaddle2() {
    return leftPaddle2(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Left Paddle 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Left Paddle 2 button's digital signal attached to
   *     the given loop.
   */
  public Trigger leftPaddle2(EventLoop loop) {
    return button(Gamepad.Button.kLeftPaddle2.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Touchpad button's digital signal.
   *
   * @return a Trigger instance representing the Touchpad button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #touchpad(EventLoop)
   */
  public Trigger touchpad() {
    return touchpad(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Touchpad button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Touchpad button's digital signal attached to the
   *     given loop.
   */
  public Trigger touchpad(EventLoop loop) {
    return button(Gamepad.Button.kTouchpad.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 2 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 2 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #misc2(EventLoop)
   */
  public Trigger misc2() {
    return misc2(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 2 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc2(EventLoop loop) {
    return button(Gamepad.Button.kMisc2.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 3 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 3 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #misc3(EventLoop)
   */
  public Trigger misc3() {
    return misc3(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 3 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc3(EventLoop loop) {
    return button(Gamepad.Button.kMisc3.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 4 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 4 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #misc4(EventLoop)
   */
  public Trigger misc4() {
    return misc4(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 4 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 4 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc4(EventLoop loop) {
    return button(Gamepad.Button.kMisc4.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 5 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 5 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #misc5(EventLoop)
   */
  public Trigger misc5() {
    return misc5(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 5 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 5 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc5(EventLoop loop) {
    return button(Gamepad.Button.kMisc5.value, loop);
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 6 button's digital signal.
   *
   * @return a Trigger instance representing the Miscellaneous 6 button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #misc6(EventLoop)
   */
  public Trigger misc6() {
    return misc6(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the Miscellaneous 6 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return a Trigger instance representing the Miscellaneous 6 button's digital signal attached to
   *     the given loop.
   */
  public Trigger misc6(EventLoop loop) {
    return button(Gamepad.Button.kMisc6.value, loop);
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param loop the event loop instance to attach the Trigger to.
   * @return a Trigger instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public Trigger leftTrigger(double threshold, EventLoop loop) {
    return axisGreaterThan(Gamepad.Axis.kLeftTrigger.value, threshold, loop);
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @return a Trigger instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the {@link CommandScheduler#getDefaultButtonLoop() default scheduler
   *     button loop}.
   */
  public Trigger leftTrigger(double threshold) {
    return leftTrigger(threshold, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @return a Trigger instance that is true when the left trigger's axis exceeds 0.5, attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger leftTrigger() {
    return leftTrigger(0.5);
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param loop the event loop instance to attach the Trigger to.
   * @return a Trigger instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public Trigger rightTrigger(double threshold, EventLoop loop) {
    return axisGreaterThan(Gamepad.Axis.kRightTrigger.value, threshold, loop);
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @return a Trigger instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the {@link CommandScheduler#getDefaultButtonLoop() default scheduler
   *     button loop}.
   */
  public Trigger rightTrigger(double threshold) {
    return rightTrigger(threshold, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @return a Trigger instance that is true when the right trigger's axis exceeds 0.5, attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger rightTrigger() {
    return rightTrigger(0.5);
  }

  /**
   * Get the X axis value of left side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  public double getLeftX() {
    return m_hid.getLeftX();
  }

  /**
   * Get the Y axis value of left side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return m_hid.getLeftY();
  }

  /**
   * Get the X axis value of right side of the controller. Right is positive.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return m_hid.getRightX();
  }

  /**
   * Get the Y axis value of right side of the controller. Back is positive.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return m_hid.getRightY();
  }

  /**
   * Get the left trigger axis value of the controller. Note that this axis is bound to the range of
   * [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getLeftTriggerAxis() {
    return m_hid.getLeftTriggerAxis();
  }

  /**
   * Get the right trigger axis value of the controller. Note that this axis is bound to the range
   * of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getRightTriggerAxis() {
    return m_hid.getRightTriggerAxis();
  }
}
