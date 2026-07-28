// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * A sequence of commands that run one after another. Each successive command only starts after its
 * predecessor completes execution. The priority of a sequence is equal to the highest priority of
 * any of its commands. If all commands in the sequence are able to run while the robot is disabled,
 * then the sequence itself will be allowed to run while the robot is disabled.
 *
 * <p>The sequence will <i>own</i> all mechanisms required by all commands in the sequence for the
 * entire duration of the sequence. This means that a mechanism owned by one command in the
 * sequence, but not by a later one, will be <i>uncommanded</i> while that later command executes.
 */
public final class SequentialGroup implements Command {
  private final String m_name;
  private final List<Command> m_commands = new ArrayList<>();
  private final Set<Mechanism> m_requirements = new HashSet<>();
  private final int m_priority;

  /**
   * Creates a new command sequence.
   *
   * @param name the name of the sequence
   * @param commands the commands to execute within the sequence
   */
  SequentialGroup(String name, List<Command> commands) {
    requireNonNullParam(name, "name", "SequentialGroup");
    requireNonNullParam(commands, "commands", "SequentialGroup");
    for (int i = 0; i < commands.size(); i++) {
      requireNonNullParam(commands.get(i), "commands[" + i + "]", "SequentialGroup");
    }

    m_name = name;
    m_commands.addAll(commands);

    for (var command : commands) {
      m_requirements.addAll(command.requirements());
    }

    m_priority =
        commands.stream().mapToInt(Command::priority).max().orElse(Command.DEFAULT_PRIORITY);
  }

  @Override
  public void run(Coroutine coroutine) {
    for (var command : m_commands) {
      coroutine.await(command);
    }
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
    return "SequentialGroup[name=" + m_name + "]";
  }

  // package-private for testing
  List<Command> getCommands() {
    return m_commands;
  }
}
