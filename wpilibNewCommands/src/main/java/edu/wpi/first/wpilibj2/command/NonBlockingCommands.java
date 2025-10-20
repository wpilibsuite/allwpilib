// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.HashSet;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * A utility class that constructs commands that preserve the default command behavior of their
 * requirements.
 */
public final class NonBlockingCommands {
  /**
   * Creates a new command that requires additional subsystems without interrupting the default
   * commands of said subsystems.
   *
   * @param command The base command
   * @param nonBlockingRequirements The requirements to add.
   * @return A new command
   */
  public static Command of(Command command, Subsystem... nonBlockingRequirements) {
    var cmd = command.deadlineFor(new RunDefaultsCommand(nonBlockingRequirements));
    cmd.addRequirements(nonBlockingRequirements);
    return cmd;
  }

  /**
   * Runs a group of commands in series, one after the other. Any subsystem required by the group
   * but not by the currently running command will run its default command.
   *
   * @param commands the commands to include
   * @return the command group
   */
  public static Command sequence(Command... commands) {
    var allReqs = new HashSet<Subsystem>();
    for (var command : commands) {
      allReqs.addAll(command.getRequirements());
    }
    var group = new SequentialCommandGroup();
    group.addRequirements();
    for (var command : commands) {
      var requirementsToIdle = new HashSet<>(allReqs);
      requirementsToIdle.removeAll(command.getRequirements());
      group.addCommands(command.deadlineFor(new RunDefaultsCommand(requirementsToIdle)));
    }
    return group.withName("NonBlockingSequentialCommandGroup");
  }

  /**
   * Runs a group of commands at the same time. Ends once all commands in the group finish. Any
   * subsystem required by the group but not by the currently running command will run its default
   * command.
   *
   * @param commands the commands to include
   * @return the command group
   */
  public static Command parallel(Command... commands) {
    var group = new ParallelCommandGroup();
    var numEnded = new AtomicInteger();
    for (var cmd : commands) {
      group.addCommands(
          cmd.finallyDo(numEnded::getAndIncrement)
              .andThen(new RunDefaultsCommand(cmd.getRequirements())));
    }
    return group
        .until(() -> numEnded.get() == commands.length)
        .withName("NonBlockingParallelCommandGroup");
  }

  /**
   * Runs a group of commands at the same time. Ends once a specific command finishes, and cancels
   * the others. Any subsystem required by the group but not by the currently running command will
   * run its default command.
   *
   * @param deadline the deadline command
   * @param otherCommands the other commands to include
   * @return the command group
   * @see ParallelDeadlineGroup
   * @throws IllegalArgumentException if the deadline command is also in the otherCommands argument
   */
  public static Command deadline(Command deadline, Command... otherCommands) {
    return parallel(otherCommands)
        .withDeadline(deadline)
        .withName("NonBlockingParallelDeadlineGroup");
  }

  private NonBlockingCommands() {
    throw new UnsupportedOperationException("This is a utility class");
  }
}
