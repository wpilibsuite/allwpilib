// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;

/**
 * A type of command that runs multiple other commands in parallel. The group will end after all
 * required commands have completed; if no command is explicitly required for completion, then the
 * group will end after any command in the group finishes. Any commands still running when the group
 * has reached its completion condition will be canceled.
 */
public final class ParallelGroup implements Command {
  private final Collection<Command> m_requiredCommands = new HashSet<>();
  private final Collection<Command> m_optionalCommands = new LinkedHashSet<>();
  private final Set<Mechanism> m_requirements = new HashSet<>();
  private final String m_name;
  private final int m_priority;

  /**
   * Creates a new parallel group.
   *
   * @param name The name of the group.
   * @param requiredCommands The commands that are required to complete for the group to finish. If
   *     this is empty, then the group will finish when <i>any</i> optional command completes.
   * @param optionalCommands The commands that do not need to complete for the group to finish. If
   *     this is empty, then the group will finish when <i>all</i> required commands complete.
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  ParallelGroup(
      String name, Collection<Command> requiredCommands, Collection<Command> optionalCommands) {
    requireNonNullParam(name, "name", "ParallelGroup");
    requireNonNullParam(requiredCommands, "requiredCommands", "ParallelGroup");
    requireNonNullParam(optionalCommands, "optionalCommands", "ParallelGroup");

    int i = 0;
    for (Command requiredCommand : requiredCommands) {
      requireNonNullParam(requiredCommand, "requiredCommands[" + i + "]", "ParallelGroup");
      i++;
    }

    i = 0;
    for (Command c : optionalCommands) {
      requireNonNullParam(c, "optionalCommands[" + i + "]", "ParallelGroup");
      i++;
    }

    var allCommands = new ArrayList<Command>();
    allCommands.addAll(requiredCommands);
    allCommands.addAll(optionalCommands);

    for (Command c1 : allCommands) {
      for (Command c2 : allCommands) {
        if (c1 == c2) {
          // Commands can't conflict with themselves
          continue;
        }
        if (c1.conflictsWith(c2)) {
          throw new IllegalArgumentException(
              "Commands in a parallel composition cannot require the same mechanisms. "
                  + c1.name()
                  + " and "
                  + c2.name()
                  + " conflict.");
        }
      }
    }

    m_name = name;
    m_optionalCommands.addAll(optionalCommands);
    m_requiredCommands.addAll(requiredCommands);

    for (var command : allCommands) {
      m_requirements.addAll(command.requirements());
    }

    m_priority =
        allCommands.stream().mapToInt(Command::priority).max().orElse(Command.DEFAULT_PRIORITY);
  }

  @Override
  public void run(Coroutine coroutine) {
    coroutine.forkAll(m_optionalCommands);

    if (m_requiredCommands.isEmpty()) {
      // No required commands - just wait for the first optional command to finish
      coroutine.awaitAny(m_optionalCommands);
    } else {
      // Wait for every required command to finish
      coroutine.awaitAll(m_requiredCommands);
    }

    // The scheduler will cancel any optional child commands that are still running when
    // the `run` method returns
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
    return "ParallelGroup[name=" + m_name + "]";
  }
}
