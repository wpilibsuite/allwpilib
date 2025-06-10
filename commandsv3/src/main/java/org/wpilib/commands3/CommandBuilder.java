// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.function.Consumer;

/**
 * A builder that allows for all functionality of a command to be configured prior to creating it.
 */
public class CommandBuilder {
  private final Set<RequireableResource> requirements = new HashSet<>();
  private Consumer<Coroutine> impl;
  private Runnable onCancel = () -> {};
  private String name;
  private int priority = Command.DEFAULT_PRIORITY;

  /**
   * Adds a resource as a requirement for the command.
   *
   * @param resource The required resource
   * @return The builder object, for chaining
   * @see Command#requirements()
   */
  public CommandBuilder requiring(RequireableResource resource) {
    requireNonNullParam(resource, "resource", "CommandBuilder.requiring");

    requirements.add(resource);
    return this;
  }

  /**
   * Adds resources as requirements for the command.
   *
   * @param resources The required resources
   * @return The builder object, for chaining
   * @see Command#requirements()
   */
  public CommandBuilder requiring(RequireableResource... resources) {
    requireNonNullParam(resources, "resources", "CommandBuilder.requiring");
    for (int i = 0; i < resources.length; i++) {
      requireNonNullParam(resources[i], "resources[" + i + "]", "CommandBuilder.requiring");
    }

    requirements.addAll(Arrays.asList(resources));
    return this;
  }

  /**
   * Adds resources as requirements for the command.
   *
   * @param resources The required resources
   * @return The builder object, for chaining
   * @see Command#requirements()
   */
  public CommandBuilder requiring(Collection<RequireableResource> resources) {
    requireNonNullParam(resources, "resources", "CommandBuilder.requiring");
    if (resources instanceof List<RequireableResource> l) {
      for (int i = 0; i < l.size(); i++) {
        requireNonNullParam(l.get(i), "resources[" + i + "]", "CommandBuilder.requiring");
      }
    } else {
      // non-indexable collection
      for (var resource : resources) {
        requireNonNullParam(resource, "resources", "CommandBuilder.requiring");
      }
    }

    requirements.addAll(resources);
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

    this.name = name;
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
    this.priority = priority;
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

    this.impl = impl;
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

    this.onCancel = onCancel;
    return this;
  }

  /**
   * Creates the command.
   *
   * @return The built command
   * @throws NullPointerException An NPE if either the command {@link #named(String) name} or {@link
   *     #executing(Consumer) implementation} were not configured before calling this method.
   */
  public Command make() {
    Objects.requireNonNull(name, "Name was not specified");
    Objects.requireNonNull(impl, "Command logic was not specified");

    return new Command() {
      @Override
      public void run(Coroutine coroutine) {
        impl.accept(coroutine);
      }

      @Override
      public void onCancel() {
        onCancel.run();
      }

      @Override
      public String name() {
        return name;
      }

      @Override
      public Set<RequireableResource> requirements() {
        return requirements;
      }

      @Override
      public int priority() {
        return priority;
      }

      @Override
      public String toString() {
        return "Command name=" + name() + " priority=" + priority;
      }
    };
  }
}
