// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

/**
 * A builder that allows for all functionality of a command to be configured prior to creating it.
 */
public class CommandBuilder {
  private final Set<Mechanism> m_requirements = new HashSet<>();
  private Consumer<Coroutine> m_impl;
  private Runnable m_onCancel = () -> {};
  private String m_name;
  private int m_priority = Command.DEFAULT_PRIORITY;
  private BooleanSupplier m_endCondition;

  /**
   * Adds a mechanism as a requirement for the command.
   *
   * @param mechanism The required mechanism
   * @return The builder object, for chaining
   * @see Command#requirements()
   */
  public CommandBuilder requiring(Mechanism mechanism) {
    requireNonNullParam(mechanism, "mechanism", "CommandBuilder.requiring");

    m_requirements.add(mechanism);
    return this;
  }

  /**
   * Adds mechanisms as requirements for the command.
   *
   * @param mechanisms The required mechanisms
   * @return The builder object, for chaining
   * @see Command#requirements()
   */
  public CommandBuilder requiring(Mechanism... mechanisms) {
    requireNonNullParam(mechanisms, "mechanisms", "CommandBuilder.requiring");
    for (int i = 0; i < mechanisms.length; i++) {
      requireNonNullParam(mechanisms[i], "mechanisms[" + i + "]", "CommandBuilder.requiring");
    }

    m_requirements.addAll(Arrays.asList(mechanisms));
    return this;
  }

  /**
   * Adds mechanisms as requirements for the command.
   *
   * @param mechanisms The required mechanisms
   * @return The builder object, for chaining
   * @see Command#requirements()
   */
  public CommandBuilder requiring(Collection<Mechanism> mechanisms) {
    requireNonNullParam(mechanisms, "mechanisms", "CommandBuilder.requiring");
    if (mechanisms instanceof List<Mechanism> l) {
      for (int i = 0; i < l.size(); i++) {
        requireNonNullParam(l.get(i), "mechanisms[" + i + "]", "CommandBuilder.requiring");
      }
    } else {
      // non-indexable collection
      for (var mechanism : mechanisms) {
        requireNonNullParam(mechanism, "mechanisms", "CommandBuilder.requiring");
      }
    }

    m_requirements.addAll(mechanisms);
    return this;
  }

  /**
   * Sets the name of the command.
   *
   * @param name The command's name
   * @return The builder object, for chaining
   * @see Command#name()
   */
  public CommandBuilder withName(String name) {
    requireNonNullParam(name, "name", "CommandBuilder.withName");

    m_name = name;
    return this;
  }

  /**
   * Creates the command based on what's been configured in the builder.
   *
   * @param name The name of the command
   * @return The built command
   * @throws NullPointerException if {@code name} is null or if the command body was not set using
   *     {@link #executing(Consumer)}.
   * @see Command#name()
   */
  public Command named(String name) {
    return withName(name).make();
  }

  /**
   * Sets the priority of the command. If not set, {@link Command#DEFAULT_PRIORITY} will be used.
   *
   * @param priority The priority of the command
   * @return The builder object, for chaining
   * @see Command#priority()
   */
  public CommandBuilder withPriority(int priority) {
    m_priority = priority;
    return this;
  }

  /**
   * Sets the code that the command will execute when it's being run by the scheduler.
   *
   * @param impl The command implementation
   * @return The builder object, for chaining
   * @see Command#run(Coroutine)
   */
  public CommandBuilder executing(Consumer<Coroutine> impl) {
    requireNonNullParam(impl, "impl", "CommandBuilder.executing");

    m_impl = impl;
    return this;
  }

  /**
   * Sets the code that the command will execute if it's cancelled while running.
   *
   * @param onCancel The cancellation callback
   * @return The builder object, for chaining.
   * @see Command#onCancel()
   */
  public CommandBuilder whenCanceled(Runnable onCancel) {
    requireNonNullParam(onCancel, "onCancel", "CommandBuilder.whenCanceled");

    m_onCancel = onCancel;
    return this;
  }

  /**
   * Makes the command run until some end condition is met, if it hasn't already finished by then on
   * its own.
   *
   * @param endCondition The hard end condition for the command.
   * @return The builder object, for chaining.
   */
  public CommandBuilder until(BooleanSupplier endCondition) {
    m_endCondition = endCondition;
    return this;
  }

  /**
   * Makes the command run while some condition is true, ending when the condition becomes inactive.
   *
   * @param active The command active condition.
   * @return The builder object, for chaining.
   */
  public CommandBuilder asLongAs(BooleanSupplier active) {
    return until(() -> !active.getAsBoolean());
  }

  /**
   * Creates the command.
   *
   * @return The built command
   * @throws NullPointerException An NPE if either the command {@link #named(String) name} or {@link
   *     #executing(Consumer) implementation} were not configured before calling this method.
   */
  public Command make() {
    Objects.requireNonNull(m_name, "Name was not specified");
    Objects.requireNonNull(m_impl, "Command logic was not specified");

    var command =
        new Command() {
          @Override
          public void run(Coroutine coroutine) {
            m_impl.accept(coroutine);
          }

          @Override
          public void onCancel() {
            m_onCancel.run();
          }

          @Override
          public String name() {
            return m_name;
          }

          @Override
          public Set<Mechanism> requirements() {
            return m_requirements;
          }

          @Override
          public int priority() {
            return m_priority;
          }

          @Override
          public String toString() {
            return m_name;
          }
        };

    if (m_endCondition == null) {
      // No custom end condition, just return the raw command
      return command;
    } else {
      // A custom end condition is implemented as a race group, since we cannot modify the command
      // body to inject the end condition.
      return new ParallelGroupBuilder().requiring(command).until(m_endCondition).named(m_name);
    }
  }
}
