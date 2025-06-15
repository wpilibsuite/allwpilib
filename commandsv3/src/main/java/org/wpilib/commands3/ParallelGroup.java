// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;

/**
 * A type of command that runs multiple other commands in parallel. The group will end after all
 * required commands have completed; if no command is explicitly required for completion, then the
 * group will end after any command in the group finishes. Any commands still running when the group
 * has reached its completion condition will be cancelled.
 */
public class ParallelGroup implements Command {
  private final Collection<Command> m_commands = new LinkedHashSet<>();
  private final Collection<Command> m_requiredCommands = new HashSet<>();
  private final Set<RequireableResource> m_requirements = new HashSet<>();
  private final String m_name;
  private final int m_priority;

  /**
   * Creates a new parallel group.
   *
   * @param name The name of the group.
   * @param allCommands All the commands to run in parallel.
   * @param requiredCommands The commands that are required to completed for the group to finish. If
   *     this is empty, then the group will finish when <i>any</i> command in it has completed.
   */
  public ParallelGroup(
      String name, Collection<Command> allCommands, Collection<Command> requiredCommands) {
    requireNonNullParam(name, "name", "ParallelGroup");
    requireNonNullParam(allCommands, "allCommands", "ParallelGroup");
    requireNonNullParam(requiredCommands, "requiredCommands", "ParallelGroup");

    allCommands.forEach(
        c -> {
          requireNonNullParam(c, "allCommands[x]", "ParallelGroup");
        });
    requiredCommands.forEach(
        c -> {
          requireNonNullParam(c, "requiredCommands[x]", "ParallelGroup");
        });

    if (!allCommands.containsAll(requiredCommands)) {
      throw new IllegalArgumentException("Required commands must also be composed");
    }

    for (Command c1 : allCommands) {
      for (Command c2 : allCommands) {
        if (c1 == c2) {
          // Commands can't conflict with themselves
          continue;
        }
        if (c1.conflictsWith(c2)) {
          throw new IllegalArgumentException(
              "Commands in a parallel composition cannot require the same resources. "
                  + c1.name()
                  + " and "
                  + c2.name()
                  + " conflict.");
        }
      }
    }

    m_name = name;
    this.m_commands.addAll(allCommands);
    this.m_requiredCommands.addAll(requiredCommands);

    for (var command : allCommands) {
      m_requirements.addAll(command.requirements());
    }

    m_priority =
        allCommands.stream().mapToInt(Command::priority).max().orElse(Command.DEFAULT_PRIORITY);
  }

  /**
   * Creates a parallel group that runs all the given commands until any of them finish.
   *
   * @param commands the commands to run in parallel
   * @return the group
   */
  public static ParallelGroupBuilder race(Command... commands) {
    return builder().optional(commands);
  }

  /**
   * Creates a parallel group that runs all the given commands until they all finish. If a command
   * finishes early, its required resources will be idle (uncommanded) until the group completes.
   *
   * @param commands the commands to run in parallel
   * @return the group
   */
  public static ParallelGroupBuilder all(Command... commands) {
    return builder().requiring(commands);
  }

  @Override
  public void run(Coroutine coroutine) {
    if (m_requiredCommands.isEmpty()) {
      // No command is explicitly required to complete
      // Schedule everything and wait for the first one to complete
      coroutine.awaitAny(m_commands);
    } else {
      // At least one command is required to complete
      // Schedule all the non-required commands (the scheduler will automatically cancel them
      // when this group completes), and await completion of all the required commands
      m_commands.forEach(coroutine.scheduler()::schedule);
      coroutine.awaitAll(m_requiredCommands);
    }
  }

  @Override
  public String name() {
    return m_name;
  }

  @Override
  public Set<RequireableResource> requirements() {
    return m_requirements;
  }

  @Override
  public int priority() {
    return m_priority;
  }

  @Override
  public String toString() {
    return "ParallelGroup[name=" + m_name + "]";
  }

  public static ParallelGroupBuilder builder() {
    return new ParallelGroupBuilder();
  }
}
