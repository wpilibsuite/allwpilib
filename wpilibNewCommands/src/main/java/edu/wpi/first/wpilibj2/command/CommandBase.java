// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.util.sendable.Sendable;

/**
 * A {@link Sendable} base class for {@link Command}s.
 *
 * <p>This class is provided by the NewCommands VendorDep
 *
 * @deprecated All functionality provided by {@link CommandBase} has been merged into {@link
 *     Command}. Use {@link Command} instead.
 */
@Deprecated(since = "2024", forRemoval = true)
@SuppressWarnings("PMD.AbstractClassWithoutAnyMethod")
public abstract class CommandBase extends Command {
  /** Default constructor. */
  public CommandBase() {
    super();
  }
}
