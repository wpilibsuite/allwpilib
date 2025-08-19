// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

/**
 * The final stage of building a command. The only options available here are setting the {@link
 * Command#onCancel()} callback and priority level; otherwise, a command may be created with {@link
 * #make()} based on the configurations applied by the previous builder stages.
 */
public interface TerminalCommandBuilder {
  /**
   * Optionally sets a callback to execute when the command is canceled. The callback will not run
   * if the command was canceled after being scheduled but before starting to run, and will not run
   * if the command completes naturally or from encountering an unhandled exception.
   *
   * @param onCancel The function to execute when the command is canceled while running. May be
   *     null.
   * @return This builder object, for chaining.
   */
  TerminalCommandBuilder whenCanceled(Runnable onCancel);

  /**
   * Sets the priority level of the command.
   *
   * @param priority The desired priority level.
   * @return This builder object, for chaining.
   */
  TerminalCommandBuilder withPriority(int priority);

  /**
   * Creates the command based on the options set during the previous builder stages. The builders
   * will no longer be usable after calling this method.
   *
   * @return The built command.
   */
  Command make();
}
