// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

/**
 * A stage in a command builder where requirements and main command execution logic have been set.
 * No more changes to requirements or command implementation may happen after this point. This is
 * the final step in command creation
 */
public interface HasExecutionCommandBuilder {
  /**
   * Optionally sets a callback to execute when the command is canceled. The callback will not run
   * if the command was canceled after being scheduled but before starting to run, and will not run
   * if the command completes naturally or from encountering an unhandled exception.
   *
   * @param onCancel The function to execute when the command is canceled while running. May be
   *     null.
   * @return This builder object, for chaining
   */
  HasExecutionCommandBuilder whenCanceled(Runnable onCancel);

  /**
   * Sets the priority level of the command.
   *
   * @param priority The desired priority level.
   * @return This builder object, for chaining.
   */
  HasExecutionCommandBuilder withPriority(int priority);

  /**
   * Optionally sets an end condition for the command. If the end condition returns {@code true}
   * before the command body set in {@link HasRequirementsCommandBuilder#executing(Consumer)} would
   * exit, the command will be cancelled by the scheduler.
   *
   * @param endCondition An optional end condition for the command. May be null.
   * @return This builder object, for chaining.
   */
  HasExecutionCommandBuilder until(BooleanSupplier endCondition);

  /**
   * Creates the command based on the options set during previous builder stages. The builders will
   * no longer be usable after calling this method.
   *
   * @param name The name of the command
   * @return The built command.
   * @throws NullPointerException If {@code name} is null.
   */
  Command named(String name);
}
