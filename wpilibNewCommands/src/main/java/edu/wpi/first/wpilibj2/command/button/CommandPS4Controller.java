// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj.PS4Controller;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

/**
 * A subclass of {@link PS4Controller} with {@link Trigger} factories for command-based.
 *
 * @see PS4Controller
 */
@SuppressWarnings("MethodName")
public class CommandPS4Controller extends PS4Controller {
  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public CommandPS4Controller(int port) {
    super(port);
  }

  /**
   * Constructs an event instance around the L2 button's digital signal.
   *
   * @return an event instance representing the L2 button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger L2() {
    return L2(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger L2(EventLoop loop) {
    return super.L2(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the R2 button's digital signal.
   *
   * @return an event instance representing the R2 button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger R2() {
    return R2(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger R2(EventLoop loop) {
    return super.R2(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the L1 button's digital signal.
   *
   * @return an event instance representing the L1 button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger L1() {
    return L1(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger L1(EventLoop loop) {
    return super.L1(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the R1 button's digital signal.
   *
   * @return an event instance representing the R1 button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger R1() {
    return R1(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger R1(EventLoop loop) {
    return super.R1(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the L3 button's digital signal.
   *
   * @return an event instance representing the L3 button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger L3() {
    return L3(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger L3(EventLoop loop) {
    return super.L3(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the R3 button's digital signal.
   *
   * @return an event instance representing the R3 button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger R3() {
    return R3(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger R3(EventLoop loop) {
    return super.R3(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the square button's digital signal.
   *
   * @return an event instance representing the square button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger square() {
    return square(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger square(EventLoop loop) {
    return super.square(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the cross button's digital signal.
   *
   * @return an event instance representing the cross button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger cross() {
    return cross(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger cross(EventLoop loop) {
    return super.cross(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the circle button's digital signal.
   *
   * @return an event instance representing the circle button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger circle() {
    return circle(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger circle(EventLoop loop) {
    return super.circle(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the share button's digital signal.
   *
   * @return an event instance representing the share button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger share() {
    return share(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger share(EventLoop loop) {
    return super.share(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the PS button's digital signal.
   *
   * @return an event instance representing the PS button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger PS() {
    return PS(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger PS(EventLoop loop) {
    return super.PS(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the options button's digital signal.
   *
   * @return an event instance representing the options button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger options() {
    return options(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger options(EventLoop loop) {
    return super.options(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the touchpad's digital signal.
   *
   * @return an event instance representing the touchpad's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   */
  public Trigger touchpad() {
    return touchpad(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger touchpad(EventLoop loop) {
    return super.touchpad(loop).castTo(Trigger::new);
  }
}
