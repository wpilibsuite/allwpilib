// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.util.Collection;
import java.util.function.Consumer;

/**
 * A stage in a command builder where requirements have already been specified. The next stage is a
 * {@link HasExecutionCommandBuilder} from {@link #executing(Consumer)}. Additional requirements may
 * still be added before moving on to the next stage.
 */
public interface HasRequirementsCommandBuilder {
  /**
   * Adds a required mechanism for the command.
   *
   * @param requirement A required mechanism. Cannot be null.
   * @return This builder object, for chaining
   * @throws NullPointerException If {@code requirement} is null
   */
  HasRequirementsCommandBuilder requiring(Mechanism requirement);

  /**
   * Adds one or more required mechanisms for the command.
   *
   * @param requirement A required mechanism. Cannot be null.
   * @param extra Any extra required mechanisms. May be empty, but cannot contain null values.
   * @return This builder object, for chaining
   * @throws NullPointerException If {@code requirement} is null or {@code extra} contains a null
   *     value
   */
  HasRequirementsCommandBuilder requiring(Mechanism requirement, Mechanism... extra);

  /**
   * Adds required mechanisms for the command.
   *
   * @param requirements Any required mechanisms. May be empty, but cannot contain null values.
   * @return This builder object, for chaining
   * @throws NullPointerException If {@code requirements} is null or contains a null value.
   */
  HasRequirementsCommandBuilder requiring(Collection<Mechanism> requirements);

  /**
   * Sets the function body of the executing command.
   *
   * @param impl The command's body. Cannot be null.
   * @return A builder for the next stage of command construction.
   * @throws NullPointerException If {@code impl} is null.
   */
  HasExecutionCommandBuilder executing(Consumer<Coroutine> impl);
}
