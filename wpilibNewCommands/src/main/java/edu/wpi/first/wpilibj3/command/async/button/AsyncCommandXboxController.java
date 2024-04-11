// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj3.command.async.button;

import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj3.command.async.AsyncScheduler;

/**
 * A version of {@link XboxController} with {@link AsyncTrigger} factories for command-based.
 *
 * @see XboxController
 */
@SuppressWarnings("MethodName")
public class AsyncCommandXboxController extends AsyncCommandGenericHID {
  private final XboxController m_hid;

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public AsyncCommandXboxController(int port) {
    super(port);
    m_hid = new XboxController(port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  @Override
  public XboxController getHID() {
    return m_hid;
  }

  /**
   * Constructs an event instance around the left bumper's digital signal.
   *
   * @return an event instance representing the left bumper's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftBumper(EventLoop)
   */
  public AsyncTrigger leftBumper() {
    return leftBumper(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the left bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger leftBumper(EventLoop loop) {
    return m_hid.leftBumper(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @return an event instance representing the right bumper's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightBumper(EventLoop)
   */
  public AsyncTrigger rightBumper() {
    return rightBumper(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left bumper's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger rightBumper(EventLoop loop) {
    return m_hid.rightBumper(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @return an event instance representing the left stick button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftStick(EventLoop)
   */
  public AsyncTrigger leftStick() {
    return leftStick(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left stick button's digital signal attached to the
   *     given loop.
   */
  public AsyncTrigger leftStick(EventLoop loop) {
    return m_hid.leftStick(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @return an event instance representing the right stick button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightStick(EventLoop)
   */
  public AsyncTrigger rightStick() {
    return rightStick(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public AsyncTrigger rightStick(EventLoop loop) {
    return m_hid.rightStick(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @return an event instance representing the A button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #a(EventLoop)
   */
  public AsyncTrigger a() {
    return a(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the A button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger a(EventLoop loop) {
    return m_hid.a(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @return an event instance representing the B button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #b(EventLoop)
   */
  public AsyncTrigger b() {
    return b(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the B button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger b(EventLoop loop) {
    return m_hid.b(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @return an event instance representing the X button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #x(EventLoop)
   */
  public AsyncTrigger x() {
    return x(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the X button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger x(EventLoop loop) {
    return m_hid.x(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @return an event instance representing the Y button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #y(EventLoop)
   */
  public AsyncTrigger y() {
    return y(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Y button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger y(EventLoop loop) {
    return m_hid.y(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the start button's digital signal.
   *
   * @return an event instance representing the start button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #start(EventLoop)
   */
  public AsyncTrigger start() {
    return start(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the start button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the start button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger start(EventLoop loop) {
    return m_hid.start(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the back button's digital signal.
   *
   * @return an event instance representing the back button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #back(EventLoop)
   */
  public AsyncTrigger back() {
    return back(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the back button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the back button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger back(EventLoop loop) {
    return m_hid.back(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link AsyncTrigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param loop the event loop instance to attach the Trigger to.
   * @return a Trigger instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public AsyncTrigger leftTrigger(double threshold, EventLoop loop) {
    return m_hid.leftTrigger(threshold, loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link AsyncTrigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @return a Trigger instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the {@link AsyncScheduler#getDefaultButtonLoop() default scheduler
   *     button loop}.
   */
  public AsyncTrigger leftTrigger(double threshold) {
    return leftTrigger(threshold, AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @return a Trigger instance that is true when the left trigger's axis exceeds 0.5, attached to
   *     the {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger leftTrigger() {
    return leftTrigger(0.5);
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link AsyncTrigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param loop the event loop instance to attach the Trigger to.
   * @return a Trigger instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public AsyncTrigger rightTrigger(double threshold, EventLoop loop) {
    return m_hid.rightTrigger(threshold, loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link AsyncTrigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @return a Trigger instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the {@link AsyncScheduler#getDefaultButtonLoop() default scheduler
   *     button loop}.
   */
  public AsyncTrigger rightTrigger(double threshold) {
    return rightTrigger(threshold, AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @return a Trigger instance that is true when the right trigger's axis exceeds 0.5, attached to
   *     the {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger rightTrigger() {
    return rightTrigger(0.5);
  }

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftX() {
    return m_hid.getLeftX();
  }

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightX() {
    return m_hid.getRightX();
  }

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return The axis value.
   */
  public double getLeftY() {
    return m_hid.getLeftY();
  }

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return The axis value.
   */
  public double getRightY() {
    return m_hid.getRightY();
  }

  /**
   * Get the left trigger (LT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getLeftTriggerAxis() {
    return m_hid.getLeftTriggerAxis();
  }

  /**
   * Get the right trigger (RT) axis value of the controller. Note that this axis is bound to the
   * range of [0, 1] as opposed to the usual [-1, 1].
   *
   * @return The axis value.
   */
  public double getRightTriggerAxis() {
    return m_hid.getRightTriggerAxis();
  }
}
