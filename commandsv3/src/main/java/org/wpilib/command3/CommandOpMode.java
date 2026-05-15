// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import org.wpilib.opmode.OpMode;

/**
 * Base opmode implementation that runs the command scheduler.
 *
 * <p>This class provides a minimal {@link OpMode} that executes the default {@link Scheduler}
 * during each periodic cycle. It is used internally by {@link CommandRobot} and {@link
 * CommandOpModes} as the opmode that commands are scheduled and run within.
 *
 * <p>Teams typically do not instantiate this class directly; instead, use {@link
 * CommandRobot#createAutoOpMode(String)}, {@link CommandRobot#createTeleopOpMode(String)}, {@link
 * CommandRobot#createUtilityOpMode(String)}, or the corresponding static methods in {@link
 * CommandOpModes} to create and register opmodes.
 *
 * @see CommandRobot
 * @see CommandOpModes
 * @see OpModeTriggers
 * @see Scheduler
 */
public class CommandOpMode implements OpMode {
  /** Constructs a command opmode. */
  public CommandOpMode() {}

  /**
   * Runs the default scheduler.
   *
   * <p>This method is called periodically by the framework during opmode execution (when the opmode
   * is loaded and active). The scheduler manages command scheduling and subsystem periodic methods.
   */
  @Override
  public void periodic() {
    Scheduler.getDefault().run();
  }
}
