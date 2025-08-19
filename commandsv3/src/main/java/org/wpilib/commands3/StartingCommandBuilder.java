// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.util.Collection;
import java.util.function.Consumer;

/**
 * The first stage in a command builder. Requirements (or lack thereof) may be specified with {@link
 * #requiring(Mechanism)}, {@link #requiring(Mechanism, Mechanism...)}, {@link
 * #requiring(Collection)}, or {@link #noRequirements()}, which goes to a {@link
 * HasRequirementsCommandBuilder} stage. {@link #withPriority(int)} may be used before requirements
 * are specified.
 */
public interface StartingCommandBuilder {
  /**
   * Explicitly marks the command as requiring no mechanisms. Unless overridden later with {@link
   * HasRequirementsCommandBuilder#requiring(Mechanism)} or a similar method, the built command will
   * not have ownership over any mechanisms when it runs. Use this for commands that don't need to
   * own a mechanism, such as a gyro zeroing command, that does some kind of cleanup task without
   * needing to control something.
   *
   * @return A builder object that can be used to further configure the command.
   */
  HasRequirementsCommandBuilder noRequirements();

  /**
   * Marks the command as requiring one or more mechanisms. If only a single mechanism is required,
   * prefer a factory function like {@link Mechanism#run(Consumer)} or similar - it will
   * automatically require the mechanism, instead of it needing to be explicitly specified.
   *
   * @param requirement The first required mechanism. Cannot be null.
   * @param extra Any optional extra required mechanisms. May be empty, but cannot be null or
   *     contain null values.
   * @return A builder object that can be used to further configure the command.
   */
  HasRequirementsCommandBuilder requiring(Mechanism requirement, Mechanism... extra);

  /**
   * Marks the command as requiring zero or more mechanisms. If only a single mechanism is required,
   * prefer a factory function like {@link Mechanism#run(Consumer)} or similar - it will
   * automatically require the mechanism, instead of it needing to be explicitly specified.
   *
   * @param requirements A collection of required mechanisms. May be empty, but cannot be null or
   *     contain null values.
   * @return A builder object that can be used to further configure the command.
   */
  HasRequirementsCommandBuilder requiring(Collection<Mechanism> requirements);

  /**
   * Sets the priority level of the command.
   *
   * @param priority The desired priority level.
   * @return This builder object, for chaining.
   */
  StartingCommandBuilder withPriority(int priority);
}
