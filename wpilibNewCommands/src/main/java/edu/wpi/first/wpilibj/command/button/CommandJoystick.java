// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command.button;

import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.command.CommandScheduler;
import edu.wpi.first.wpilibj.event.EventLoop;

/**
 * A subclass of {@link Joystick} with {@link Trigger} factories for command-based.
 *
 * @see Joystick
 */
public class CommandJoystick extends Joystick {
  /**
   * Construct an instance of a controller.
   *
   * @param port The port index on the Driver Station that the controller is plugged into.
   */
  public CommandJoystick(int port) {
    super(port);
  }

  @Override
  public Trigger button(int button, EventLoop loop) {
    return super.button(button, loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @return an event instance representing the button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #button(int, EventLoop)
   */
  public Trigger button(int button) {
    return button(button, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around the trigger button's digital signal.
   *
   * @return an event instance representing the trigger button's digital signal attached to the
   *     {@link CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #trigger(EventLoop)
   */
  public Trigger trigger() {
    return trigger(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger trigger(EventLoop loop) {
    return super.trigger(loop).castTo(Trigger::new);
  }

  /**
   * Constructs an event instance around the top button's digital signal.
   *
   * @return an event instance representing the top button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #top(EventLoop)
   */
  public Trigger top() {
    return top(CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  @Override
  public Trigger top(EventLoop loop) {
    return super.top(loop).castTo(Trigger::new);
  }
}
