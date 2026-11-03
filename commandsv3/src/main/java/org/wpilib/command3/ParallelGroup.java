// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

/**
 * A type of command that runs multiple other commands in parallel. The group will end after all
 * required commands have completed; if no command is explicitly required for completion, then the
 * group will end after any command in the group finishes. Any commands still running when the group
 * has reached its completion condition will be canceled.
 */
public final class ParallelGroup implements Command {
  private final Collection<Command> m_requiredCommands = new LinkedHashSet<>();
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
  ParallelGroup(
      String name, Collection<Command> requiredCommands, Collection<Command> optionalCommands) {
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

    var allCommands = new LinkedHashSet<Command>();
    allCommands.addAll(requiredCommands);
    allCommands.addAll(optionalCommands);

    ConflictDetector.throwIfConflicts(allCommands);

    m_optionalCommands.addAll(optionalCommands);
    m_requiredCommands.addAll(requiredCommands);
    m_name = name == null ? computeAutomaticName() : name;

    for (var command : allCommands) {
      m_requirements.addAll(command.requirements());
    }

    m_priority =
        allCommands.stream().mapToInt(Command::priority).max().orElse(Command.DEFAULT_PRIORITY);
  }

  /**
   * Adds one or more required commands to the group. All required commands will need to complete
   * for the group to exit.
   *
   * @param parallel The required commands to add to the group
   * @return A new {@link ParallelGroup}
   */
  public ParallelGroup requiring(Command... parallel) {
    requireNonNullParam(parallel, "parallel", "ParallelGroup.requiring");

    var commands = new Command[m_requiredCommands.size() + parallel.length];
    m_requiredCommands.toArray(commands);
    System.arraycopy(parallel, 0, commands, m_requiredCommands.size(), parallel.length);
    return new ParallelGroup(null, List.of(commands), m_optionalCommands);
  }

  /**
   * Adds one or more optional commands to the group. They will not be required to complete for the
   * parallel group to exit, and will be canceled once all required commands have finished.
   *
   * @param parallel The optional commands to add to the group
   * @return A new {@link ParallelGroup}
   */
  public ParallelGroup optional(Command... parallel) {
    requireNonNullParam(parallel, "parallel", "ParallelGroup.optional");

    var commands = new Command[m_optionalCommands.size() + parallel.length];
    m_optionalCommands.toArray(commands);
    System.arraycopy(parallel, 0, commands, m_optionalCommands.size(), parallel.length);
    return new ParallelGroup(null, m_requiredCommands, List.of(commands));
  }

  /**
   * Creates a parallel command group, running this command alongside one or more other commands.
   * The group will exit once every command has finished.
   *
   * @param parallel The commands to run in parallel with this one
   * @return A parallel command group
   */
  @Override
  public ParallelGroup alongWith(Command... parallel) {
    return requiring(parallel);
  }

  @Override
  public void run(Coroutine coroutine) {
    coroutine.fork(m_optionalCommands);

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

  private String computeAutomaticName() {
    // eg "(CommandA & CommandB & CommandC)"
    String required =
        m_requiredCommands.stream().map(Command::name).collect(Collectors.joining(" & ", "(", ")"));

    // eg "(CommandA | CommandB | CommandC)"
    String optional =
        m_optionalCommands.stream().map(Command::name).collect(Collectors.joining(" | ", "(", ")"));

    if (m_requiredCommands.isEmpty()) {
      // No required commands, pure race
      return optional;
    } else if (m_optionalCommands.isEmpty()) {
      // Everything required
      return required;
    } else {
      // Some form of deadline
      // eg "[(CommandA & CommandB) * (CommandX | CommandY | ...)]"
      return "[%s * %s]".formatted(required, optional);
    }
  }

  // package-private for testing

  Collection<Command> getRequiredCommands() {
    return m_requiredCommands;
  }

  Collection<Command> getOptionalCommands() {
    return m_optionalCommands;
  }
}
