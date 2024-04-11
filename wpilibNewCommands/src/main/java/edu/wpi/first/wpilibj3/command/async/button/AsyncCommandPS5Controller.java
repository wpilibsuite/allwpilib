// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj3.command.async.button;

import edu.wpi.first.wpilibj.PS5Controller;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj3.command.async.AsyncScheduler;

/**
 * A version of {@link PS5Controller} with {@link AsyncTrigger} factories for command-based.
 *
 * @see PS5Controller
 */
@SuppressWarnings("MethodName")
public class AsyncCommandPS5Controller extends AsyncCommandGenericHID {
  private final PS5Controller m_hid;

  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public AsyncCommandPS5Controller(int port) {
    super(port);
    m_hid = new PS5Controller(port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  @Override
  public PS5Controller getHID() {
    return m_hid;
  }

  /**
   * Constructs an event instance around the L2 button's digital signal.
   *
   * @return an event instance representing the L2 button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger L2() {
    return L2(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the L2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L2 button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger L2(EventLoop loop) {
    return m_hid.L2(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the R2 button's digital signal.
   *
   * @return an event instance representing the R2 button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger R2() {
    return R2(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the R2 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R2 button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger R2(EventLoop loop) {
    return m_hid.R2(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the L1 button's digital signal.
   *
   * @return an event instance representing the L1 button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger L1() {
    return L1(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the L1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L1 button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger L1(EventLoop loop) {
    return m_hid.L1(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the R1 button's digital signal.
   *
   * @return an event instance representing the R1 button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger R1() {
    return R1(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the R1 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R1 button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger R1(EventLoop loop) {
    return m_hid.R1(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the L3 button's digital signal.
   *
   * @return an event instance representing the L3 button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger L3() {
    return L3(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the L3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the L3 button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger L3(EventLoop loop) {
    return m_hid.L3(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the R3 button's digital signal.
   *
   * @return an event instance representing the R3 button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger R3() {
    return R3(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the R3 button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the R3 button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger R3(EventLoop loop) {
    return m_hid.R3(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the square button's digital signal.
   *
   * @return an event instance representing the square button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger square() {
    return square(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the square button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the square button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger square(EventLoop loop) {
    return m_hid.square(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the cross button's digital signal.
   *
   * @return an event instance representing the cross button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger cross() {
    return cross(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the cross button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the cross button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger cross(EventLoop loop) {
    return m_hid.cross(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the triangle button's digital signal.
   *
   * @return an event instance representing the triangle button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger triangle() {
    return triangle(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the triangle button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the triangle button's digital signal attached to the
   *     given loop.
   */
  public AsyncTrigger triangle(EventLoop loop) {
    return m_hid.triangle(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the circle button's digital signal.
   *
   * @return an event instance representing the circle button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger circle() {
    return circle(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the circle button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the circle button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger circle(EventLoop loop) {
    return m_hid.circle(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the create button's digital signal.
   *
   * @return an event instance representing the create button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger create() {
    return create(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the create button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the create button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger create(EventLoop loop) {
    return m_hid.create(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the PS button's digital signal.
   *
   * @return an event instance representing the PS button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger PS() {
    return PS(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the PS button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the PS button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger PS(EventLoop loop) {
    return m_hid.PS(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the options button's digital signal.
   *
   * @return an event instance representing the options button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger options() {
    return options(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the options button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the options button's digital signal attached to the
   *     given loop.
   */
  public AsyncTrigger options(EventLoop loop) {
    return m_hid.options(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the touchpad's digital signal.
   *
   * @return an event instance representing the touchpad's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public AsyncTrigger touchpad() {
    return touchpad(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the touchpad's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the touchpad's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger touchpad(EventLoop loop) {
    return m_hid.touchpad(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Get the X axis value of left side of the controller.
   *
   * @return the axis value.
   */
  public double getLeftX() {
    return m_hid.getLeftX();
  }

  /**
   * Get the X axis value of right side of the controller.
   *
   * @return the axis value.
   */
  public double getRightX() {
    return m_hid.getRightX();
  }

  /**
   * Get the Y axis value of left side of the controller.
   *
   * @return the axis value.
   */
  public double getLeftY() {
    return m_hid.getLeftY();
  }

  /**
   * Get the Y axis value of right side of the controller.
   *
   * @return the axis value.
   */
  public double getRightY() {
    return m_hid.getRightY();
  }

  /**
   * Get the L2 axis value of the controller. Note that this axis is bound to the range of [0, 1] as
   * opposed to the usual [-1, 1].
   *
   * @return the axis value.
   */
  public double getL2Axis() {
    return m_hid.getL2Axis();
  }

  /**
   * Get the R2 axis value of the controller. Note that this axis is bound to the range of [0, 1] as
   * opposed to the usual [-1, 1].
   *
   * @return the axis value.
   */
  public double getR2Axis() {
    return m_hid.getR2Axis();
  }
}
