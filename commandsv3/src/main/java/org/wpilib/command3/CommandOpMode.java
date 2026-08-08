// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import org.wpilib.opmode.OpMode;

/**
 * Base opmode implementation for mixed command-based and periodic opmode projects.
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
}
