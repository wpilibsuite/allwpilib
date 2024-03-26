// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj.StadiaController;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

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
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftBumper(EventLoop)
   */
  public Trigger leftBumper() {
    return leftBumper(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the left bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right bumper's digital signal attached to the given
   *     loop.
   */
  public Trigger leftBumper(EventLoop loop) {
    return m_hid.leftBumper(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @return an event instance representing the right bumper's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightBumper(EventLoop)
   */
  public Trigger rightBumper() {
    return rightBumper(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left bumper's digital signal attached to the given
   *     loop.
   */
  public Trigger rightBumper(EventLoop loop) {
    return m_hid.rightBumper(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @return an event instance representing the left stick button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftStick(EventLoop)
   */
  public Trigger leftStick() {
    return leftStick(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger leftStick(EventLoop loop) {
    return m_hid.leftStick(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @return an event instance representing the right stick button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightStick(EventLoop)
   */
  public Trigger rightStick() {
    return rightStick(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger rightStick(EventLoop loop) {
    return m_hid.rightStick(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the right trigger button's digital signal.
   *
   * @return an event instance representing the right trigger button's digital signal attached to
   *     the {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #rightTrigger(EventLoop)
   */
  public Trigger rightTrigger() {
    return rightTrigger(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the right trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the right trigger button's digital signal attached to
   *     the given loop.
   */
  public Trigger rightTrigger(EventLoop loop) {
    return m_hid.rightTrigger(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the left trigger button's digital signal.
   *
   * @return an event instance representing the left trigger button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #leftTrigger(EventLoop)
   */
  public Trigger leftTrigger() {
    return leftTrigger(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the left trigger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the left trigger button's digital signal attached to the
   *     given loop.
   */
  public Trigger leftTrigger(EventLoop loop) {
    return m_hid.leftTrigger(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @return an event instance representing the A button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #a(EventLoop)
   */
  public Trigger a() {
    return a(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the A button's digital signal attached to the given
   *     loop.
   */
  public Trigger a(EventLoop loop) {
    return m_hid.a(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @return an event instance representing the B button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #b(EventLoop)
   */
  public Trigger b() {
    return b(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the B button's digital signal attached to the given
   *     loop.
   */
  public Trigger b(EventLoop loop) {
    return m_hid.b(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @return an event instance representing the X button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #x(EventLoop)
   */
  public Trigger x() {
    return x(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the X button's digital signal attached to the given
   *     loop.
   */
  public Trigger x(EventLoop loop) {
    return m_hid.x(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @return an event instance representing the Y button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #y(EventLoop)
   */
  public Trigger y() {
    return y(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the Y button's digital signal attached to the given
   *     loop.
   */
  public Trigger y(EventLoop loop) {
    return m_hid.y(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the ellipses button's digital signal.
   *
   * @return an event instance representing the ellipses button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #ellipses(EventLoop)
   */
  public Trigger ellipses() {
    return ellipses(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the ellipses button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the ellipses button's digital signal attached to the
   *     given loop.
   */
  public Trigger ellipses(EventLoop loop) {
    return m_hid.ellipses(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the stadia button's digital signal.
   *
   * @return an event instance representing the stadia button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #stadia(EventLoop)
   */
  public Trigger stadia() {
    return stadia(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the stadia button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the stadia button's digital signal attached to the given
   *     loop.
   */
  public Trigger stadia(EventLoop loop) {
    return m_hid.stadia(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the google button's digital signal.
   *
   * @return an event instance representing the google button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #google(EventLoop)
   */
  public Trigger google() {
    return google(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the google button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the google button's digital signal attached to the given
   *     loop.
   */
  public Trigger google(EventLoop loop) {
    return m_hid.google(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the frame button's digital signal.
   *
   * @return an event instance representing the frame button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #frame(EventLoop)
   */
  public Trigger frame() {
    return frame(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the frame button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the frame button's digital signal attached to the given
   *     loop.
   */
  public Trigger frame(EventLoop loop) {
    return m_hid.frame(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the hamburger button's digital signal.
   *
   * @return an event instance representing the hamburger button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #hamburger(EventLoop)
   */
  public Trigger hamburger() {
    return hamburger(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the hamburger button's digital signal.
   *
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the hamburger button's digital signal attached to the
   *     given loop.
   */
  public Trigger hamburger(EventLoop loop) {
    return m_hid.hamburger(loop).castTo(Trigger::new);
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
