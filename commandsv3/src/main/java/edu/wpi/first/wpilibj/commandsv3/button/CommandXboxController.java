// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3.button;

import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.commandsv3.Scheduler;

/**
 * A version of {@link XboxController} with {@link Trigger} factories for command-based.
 *
 * @see XboxController
 */
@SuppressWarnings("MethodName")
public class CommandXboxController extends CommandGenericHID {
  private final XboxController m_hid;

  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandXboxController(int port) {
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
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #leftBumper(Scheduler)
   */
  public Trigger leftBumper() {
    return leftBumper(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the left bumper's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
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
   * @see #rightBumper(Scheduler)
   */
  public Trigger rightBumper() {
    return rightBumper(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the right bumper's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
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
   * @see #leftStick(Scheduler)
   */
  public Trigger leftStick() {
    return leftStick(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the left stick button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
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
   * @see #rightStick(Scheduler)
   */
  public Trigger rightStick() {
    return rightStick(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the right stick button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the right stick button's digital signal attached to the
   *     given loop.
   */
  public Trigger rightStick(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.rightStick(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @return an event instance representing the A button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #a(Scheduler)
   */
  public Trigger a() {
    return a(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the A button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
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
   * @see #b(Scheduler)
   */
  public Trigger b() {
    return b(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the B button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
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
   * @see #x(Scheduler)
   */
  public Trigger x() {
    return x(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the X button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
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
   * @see #y(Scheduler)
   */
  public Trigger y() {
    return y(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the Y button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the Y button's digital signal attached to the given
   *     loop.
   */
  public Trigger y(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.y(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the start button's digital signal.
   *
   * @return an event instance representing the start button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #start(Scheduler)
   */
  public Trigger start() {
    return start(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the start button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the start button's digital signal attached to the given
   *     loop.
   */
  public Trigger start(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.start(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the back button's digital signal.
   *
   * @return an event instance representing the back button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   * @see #back(Scheduler)
   */
  public Trigger back() {
    return back(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the back button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the back button's digital signal attached to the given
   *     loop.
   */
  public Trigger back(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.back(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @param scheduler the event loop instance to attach the Trigger to.
   * @return a Trigger instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public Trigger leftTrigger(double threshold, Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.leftTrigger(threshold, scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @return a Trigger instance that is true when the left trigger's axis exceeds the provided
   *     threshold, attached to the {@link Scheduler#getDefaultEventLoop() default scheduler button
   *     loop}.
   */
  public Trigger leftTrigger(double threshold) {
    return leftTrigger(threshold, Scheduler.getInstance());
  }

  /**
   * Constructs a Trigger instance around the axis value of the left trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @return a Trigger instance that is true when the left trigger's axis exceeds 0.5, attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
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
   * @param scheduler the event loop instance to attach the Trigger to.
   * @return a Trigger instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the given event loop
   */
  public Trigger rightTrigger(double threshold, Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.rightTrigger(threshold, scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than {@code threshold}.
   *
   * @param threshold the minimum axis value for the returned {@link Trigger} to be true. This value
   *     should be in the range [0, 1] where 0 is the unpressed state of the axis.
   * @return a Trigger instance that is true when the right trigger's axis exceeds the provided
   *     threshold, attached to the {@link Scheduler#getDefaultEventLoop() default scheduler button
   *     loop}.
   */
  public Trigger rightTrigger(double threshold) {
    return rightTrigger(threshold, Scheduler.getInstance());
  }

  /**
   * Constructs a Trigger instance around the axis value of the right trigger. The returned trigger
   * will be true when the axis value is greater than 0.5.
   *
   * @return a Trigger instance that is true when the right trigger's axis exceeds 0.5, attached to
   *     the {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger rightTrigger() {
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
