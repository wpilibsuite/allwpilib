// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3.button;

import edu.wpi.first.wpilibj.StadiaController;
import edu.wpi.first.wpilibj.commandsv3.Scheduler;

/**
 * A version of {@link StadiaController} with {@link Trigger} factories for command-based.
 *
 * @see StadiaController
 */
@SuppressWarnings("MethodName")
public class CommandStadiaController extends CommandGenericHID {
  private final StadiaController m_hid;

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandStadiaController(int port) {
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
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #leftBumper(EventLoop)
   */
  public Trigger leftBumper() {
    return leftBumper(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the left bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper's digital signal attached to the given
   *     loop.
   */
  public Trigger leftBumper(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.leftBumper(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @return an event instance representing the right bumper's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #rightBumper(EventLoop)
   */
  public Trigger rightBumper() {
    return rightBumper(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left bumper's digital signal attached to the given
   *     loop.
   */
  public Trigger rightBumper(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.rightBumper(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @return an event instance representing the left stick button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #leftStick(EventLoop)
   */
  public Trigger leftStick() {
    return leftStick(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger leftStick(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.leftStick(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @return an event instance representing the right stick button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #rightStick(EventLoop)
   */
  public Trigger rightStick() {
    return rightStick(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger rightStick(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.rightStick(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the right trigger button's digital signal.
   *
   * @return an event instance representing the right trigger button's digital signal attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #rightTrigger(EventLoop)
   */
  public Trigger rightTrigger() {
    return rightTrigger(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the right trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right trigger button's digital signal attached to
   *     the given loop.
   */
  public Trigger rightTrigger(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.rightTrigger(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the left trigger button's digital signal.
   *
   * @return an event instance representing the left trigger button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #leftTrigger(EventLoop)
   */
  public Trigger leftTrigger() {
    return leftTrigger(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the left trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left trigger button's digital signal attached to the
   *     given loop.
   */
  public Trigger leftTrigger(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.leftTrigger(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @return an event instance representing the A button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #a(EventLoop)
   */
  public Trigger a() {
    return a(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the A button's digital signal attached to the given
   *     loop.
   */
  public Trigger a(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.a(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @return an event instance representing the B button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #b(EventLoop)
   */
  public Trigger b() {
    return b(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the B button's digital signal attached to the given
   *     loop.
   */
  public Trigger b(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.b(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @return an event instance representing the X button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #x(EventLoop)
   */
  public Trigger x() {
    return x(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the X button's digital signal attached to the given
   *     loop.
   */
  public Trigger x(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.x(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @return an event instance representing the Y button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #y(EventLoop)
   */
  public Trigger y() {
    return y(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Y button's digital signal attached to the given
   *     loop.
   */
  public Trigger y(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.y(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the ellipses button's digital signal.
   *
   * @return an event instance representing the ellipses button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #ellipses(EventLoop)
   */
  public Trigger ellipses() {
    return ellipses(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the ellipses button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the ellipses button's digital signal attached to the
   *     given loop.
   */
  public Trigger ellipses(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.ellipses(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the stadia button's digital signal.
   *
   * @return an event instance representing the stadia button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #stadia(EventLoop)
   */
  public Trigger stadia() {
    return stadia(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the stadia button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the stadia button's digital signal attached to the given
   *     loop.
   */
  public Trigger stadia(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.stadia(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the google button's digital signal.
   *
   * @return an event instance representing the google button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #google(EventLoop)
   */
  public Trigger google() {
    return google(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the google button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the google button's digital signal attached to the given
   *     loop.
   */
  public Trigger google(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.google(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the frame button's digital signal.
   *
   * @return an event instance representing the frame button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #frame(EventLoop)
   */
  public Trigger frame() {
    return frame(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the frame button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the frame button's digital signal attached to the given
   *     loop.
   */
  public Trigger frame(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.frame(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the hamburger button's digital signal.
   *
   * @return an event instance representing the hamburger button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #hamburger(EventLoop)
   */
  public Trigger hamburger() {
    return hamburger(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the hamburger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the hamburger button's digital signal attached to the
   *     given loop.
   */
  public Trigger hamburger(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.hamburger(scheduler.getDefaultEventLoop()));
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
