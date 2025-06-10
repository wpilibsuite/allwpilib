// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.button;

import edu.wpi.first.wpilibj.PS4Controller;
import org.wpilib.commands3.Scheduler;
import org.wpilib.commands3.Trigger;

/**
 * A version of {@link PS4Controller} with {@link Trigger} factories for command-based.
 *
 * @see PS4Controller
 */
@SuppressWarnings("MethodName")
public class CommandPS4Controller extends CommandGenericHID {
  private final PS4Controller m_hid;

  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public CommandPS4Controller(int port) {
    super(port);
    m_hid = new PS4Controller(port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  @Override
  public PS4Controller getHID() {
    return m_hid;
  }

  /**
   * Constructs an event instance around the L2 button's digital signal.
   *
   * @return an event instance representing the L2 button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger L2() {
    return L2(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the L2 button's digital signal.
   *
   * @param scheduler the event scheduler instance to attach the event to.
   * @return an event instance representing the L2 button's digital signal attached to the given
   *     scheduler.
   */
  public Trigger L2(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.L2(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the R2 button's digital signal.
   *
   * @return an event instance representing the R2 button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger R2() {
    return R2(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the R2 button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the R2 button's digital signal attached to the given
   *     loop.
   */
  public Trigger R2(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.R2(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the L1 button's digital signal.
   *
   * @return an event instance representing the L1 button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger L1() {
    return L1(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the L1 button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the L1 button's digital signal attached to the given
   *     loop.
   */
  public Trigger L1(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.L1(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the R1 button's digital signal.
   *
   * @return an event instance representing the R1 button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger R1() {
    return R1(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the R1 button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the R1 button's digital signal attached to the given
   *     loop.
   */
  public Trigger R1(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.R1(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the L3 button's digital signal.
   *
   * @return an event instance representing the L3 button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger L3() {
    return L3(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the L3 button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the L3 button's digital signal attached to the given
   *     loop.
   */
  public Trigger L3(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.L3(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the R3 button's digital signal.
   *
   * @return an event instance representing the R3 button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger R3() {
    return R3(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the R3 button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the R3 button's digital signal attached to the given
   *     loop.
   */
  public Trigger R3(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.R3(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the square button's digital signal.
   *
   * @return an event instance representing the square button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger square() {
    return square(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the square button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the square button's digital signal attached to the given
   *     loop.
   */
  public Trigger square(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.square(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the cross button's digital signal.
   *
   * @return an event instance representing the cross button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger cross() {
    return cross(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the cross button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the cross button's digital signal attached to the given
   *     loop.
   */
  public Trigger cross(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.cross(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the triangle button's digital signal.
   *
   * @return an event instance representing the triangle button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger triangle() {
    return triangle(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the triangle button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the triangle button's digital signal attached to the
   *     given loop.
   */
  public Trigger triangle(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.triangle(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the circle button's digital signal.
   *
   * @return an event instance representing the circle button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger circle() {
    return circle(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the circle button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the circle button's digital signal attached to the given
   *     loop.
   */
  public Trigger circle(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.circle(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the share button's digital signal.
   *
   * @return an event instance representing the share button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger share() {
    return share(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the share button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the share button's digital signal attached to the given
   *     loop.
   */
  public Trigger share(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.share(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the PS button's digital signal.
   *
   * @return an event instance representing the PS button's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger PS() {
    return PS(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the PS button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the PS button's digital signal attached to the given
   *     loop.
   */
  public Trigger PS(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.PS(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the options button's digital signal.
   *
   * @return an event instance representing the options button's digital signal attached to the
   *     {@link Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger options() {
    return options(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the options button's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the options button's digital signal attached to the
   *     given loop.
   */
  public Trigger options(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.options(scheduler.getDefaultEventLoop()));
  }

  /**
   * Constructs an event instance around the touchpad's digital signal.
   *
   * @return an event instance representing the touchpad's digital signal attached to the {@link
   *     Scheduler#getDefaultEventLoop() default scheduler button loop}.
   */
  public Trigger touchpad() {
    return touchpad(Scheduler.getInstance());
  }

  /**
   * Constructs an event instance around the touchpad's digital signal.
   *
   * @param scheduler the event loop instance to attach the event to.
   * @return an event instance representing the touchpad's digital signal attached to the given
   *     loop.
   */
  public Trigger touchpad(Scheduler scheduler) {
    return new Trigger(scheduler, m_hid.touchpad(scheduler.getDefaultEventLoop()));
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
