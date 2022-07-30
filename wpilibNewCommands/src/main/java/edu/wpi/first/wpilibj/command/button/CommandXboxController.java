// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command.button;

import edu.wpi.first.wpilibj.XboxController;
import edu.wpi.first.wpilibj.command.CommandScheduler;
import edu.wpi.first.wpilibj.event.EventLoop;

/**
 * A subclass of {@link XboxController} with {@link Trigger} factories for command-based.
 *
 * @see XboxController
 */
@SuppressWarnings("MethodName")
public class CommandXboxController extends XboxController {
  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandXboxController(int port) {
    super(port);
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

  @Override
  public Trigger leftBumper(EventLoop loop) {
    return super.leftBumper(loop).castTo(Trigger::new);
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

  @Override
  public Trigger rightBumper(EventLoop loop) {
    return super.rightBumper(loop).castTo(Trigger::new);
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

  @Override
  public Trigger leftStick(EventLoop loop) {
    return super.leftStick(loop).castTo(Trigger::new);
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

  @Override
  public Trigger rightStick(EventLoop loop) {
    return super.rightStick(loop).castTo(Trigger::new);
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

  @Override
  public Trigger x(EventLoop loop) {
    return super.x(loop).castTo(Trigger::new);
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

  @Override
  public Trigger y(EventLoop loop) {
    return super.y(loop).castTo(Trigger::new);
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

  @Override
  public Trigger a(EventLoop loop) {
    return super.a(loop).castTo(Trigger::new);
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

  @Override
  public Trigger b(EventLoop loop) {
    return super.b(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the start button's digital signal.
   *
   * @return an event instance representing the start button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #start(EventLoop)
   */
  public Trigger start() {
    return start(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger start(EventLoop loop) {
    return super.start(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the back button's digital signal.
   *
   * @return an event instance representing the back button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #back(EventLoop)
   */
  public Trigger back() {
    return back(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger back(EventLoop loop) {
    return super.back(loop).castTo(Trigger::new);
  }
}
