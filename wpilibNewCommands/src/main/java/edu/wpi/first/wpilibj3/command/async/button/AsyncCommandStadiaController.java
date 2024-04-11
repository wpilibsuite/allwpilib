// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj3.command.async.button;

import edu.wpi.first.wpilibj.StadiaController;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj3.command.async.AsyncScheduler;

/**
 * A version of {@link StadiaController} with {@link AsyncTrigger} factories for command-based.
 *
 * @see StadiaController
 */
@SuppressWarnings("MethodName")
public class AsyncCommandStadiaController extends AsyncCommandGenericHID {
  private final StadiaController m_hid;

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public AsyncCommandStadiaController(int port) {
    super(port);
    m_hid = new StadiaController(port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  @Override
  public StadiaController getHID() {
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
   * Constructs an event instance around the right trigger button's digital signal.
   *
   * @return an event instance representing the right trigger button's digital signal attached to
   *     the {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightTrigger(EventLoop)
   */
  public AsyncTrigger rightTrigger() {
    return rightTrigger(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the right trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right trigger button's digital signal attached to
   *     the given loop.
   */
  public AsyncTrigger rightTrigger(EventLoop loop) {
    return m_hid.rightTrigger(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the left trigger button's digital signal.
   *
   * @return an event instance representing the left trigger button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftTrigger(EventLoop)
   */
  public AsyncTrigger leftTrigger() {
    return leftTrigger(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the left trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left trigger button's digital signal attached to the
   *     given loop.
   */
  public AsyncTrigger leftTrigger(EventLoop loop) {
    return m_hid.leftTrigger(loop).castTo(AsyncTrigger::new);
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
   * Constructs an event instance around the ellipses button's digital signal.
   *
   * @return an event instance representing the ellipses button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #ellipses(EventLoop)
   */
  public AsyncTrigger ellipses() {
    return ellipses(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the ellipses button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the ellipses button's digital signal attached to the
   *     given loop.
   */
  public AsyncTrigger ellipses(EventLoop loop) {
    return m_hid.ellipses(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the stadia button's digital signal.
   *
   * @return an event instance representing the stadia button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #stadia(EventLoop)
   */
  public AsyncTrigger stadia() {
    return stadia(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the stadia button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the stadia button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger stadia(EventLoop loop) {
    return m_hid.stadia(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the google button's digital signal.
   *
   * @return an event instance representing the google button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #google(EventLoop)
   */
  public AsyncTrigger google() {
    return google(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the google button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the google button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger google(EventLoop loop) {
    return m_hid.google(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the frame button's digital signal.
   *
   * @return an event instance representing the frame button's digital signal attached to the {@link
   *     AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #frame(EventLoop)
   */
  public AsyncTrigger frame() {
    return frame(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the frame button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the frame button's digital signal attached to the given
   *     loop.
   */
  public AsyncTrigger frame(EventLoop loop) {
    return m_hid.frame(loop).castTo(AsyncTrigger::new);
  }

  /**
   * Constructs an event instance around the hamburger button's digital signal.
   *
   * @return an event instance representing the hamburger button's digital signal attached to the
   *     {@link AsyncScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #hamburger(EventLoop)
   */
  public AsyncTrigger hamburger() {
    return hamburger(AsyncScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the hamburger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the hamburger button's digital signal attached to the
   *     given loop.
   */
  public AsyncTrigger hamburger(EventLoop loop) {
    return m_hid.hamburger(loop).castTo(AsyncTrigger::new);
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
}
