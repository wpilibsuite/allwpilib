// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command.button;

import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.command.CommandScheduler;
import edu.wpi.first.wpilibj.event.EventLoop;

/**
 * A subclass of {@link GenericHID} with {@link Trigger} factories for command-based.
 *
 * @see GenericHID
 */
public class CommandGenericHID extends GenericHID {
  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public CommandGenericHID(int port) {
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
}
