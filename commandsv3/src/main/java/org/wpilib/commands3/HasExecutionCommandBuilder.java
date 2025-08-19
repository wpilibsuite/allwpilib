// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

/**
 * A stage in a command builder where requirements and main command execution logic have been set.
 * No more changes to requirements or command implementation may happen after this point. The next
 * stage is a {@link TerminalCommandBuilder} with {@link #withName(String)}, or straight to a new
 * command using {@link #named(String)} as shorthand for {@code withName(name).make()}.
 */
public interface HasExecutionCommandBuilder {
  /**
   * Optionally sets a callback to execute when the command is canceled. The callback will not run
   * if the command was canceled after being scheduled but before starting to run, and will not run
   * if the command completes naturally or from encountering an unhandled exception.
   *
   * @param onCancel The function to execute when the command is canceled while running.
   * @return This builder object, for chaining
   * @throws NullPointerException If {@code onCancel} is null.
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
   * Sets the name of the command. {@link #named(String)} can be used instead if no other
   * configuration is necessary after this point.
   *
   * @param name The name of the command. Cannot be null.
   * @return A terminal builder object that can be used to create the command.
   * @throws NullPointerException If {@code name} is null.
   */
  TerminalCommandBuilder withName(String name);

  /**
   * Creates the command based on the options set during the previous builder stages. The builders
   * will no longer be usable after calling this method. Shorthand for {@code
   * withName(name).make()}.
   *
   * @param name The name of the command
   * @return The built command.
   * @throws NullPointerException If {@code name} is null.
   */
  default Command named(String name) {
    return withName(name).make();
  }
}
