// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.Collection;
import java.util.Collections;
import java.util.Set;
import java.util.WeakHashMap;

/**
 * A base for CommandGroups. Statically tracks commands that have been allocated to groups to ensure
 * those commands are not also used independently, which can result in inconsistent command state
 * and unpredictable execution.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public abstract class CommandGroupBase extends CommandBase {
  private static final Set<Command> m_groupedCommands =
      Collections.newSetFromMap(new WeakHashMap<>());

  static void registerGroupedCommands(Command... commands) {
    m_groupedCommands.addAll(Set.of(commands));
  }

  /**
   * Clears the list of grouped commands, allowing all commands to be freely used again.
   *
   * <p>WARNING: Using this haphazardly can result in unexpected/undesirable behavior. Do not use
   * this unless you fully understand what you are doing.
   */
  public static void clearGroupedCommands() {
    m_groupedCommands.clear();
  }

  /**
   * Removes a single command from the list of grouped commands, allowing it to be freely used
   * again.
   *
   * <p>WARNING: Using this haphazardly can result in unexpected/undesirable behavior. Do not use
   * this unless you fully understand what you are doing.
   *
   * @param command the command to remove from the list of grouped commands
   */
  public static void clearGroupedCommand(Command command) {
    m_groupedCommands.remove(command);
  }

  /**
   * Requires that the specified commands not have been already allocated to a CommandGroup. Throws
   * an {@link IllegalArgumentException} if commands have been allocated.
   *
   * @param commands The commands to check
   */
  public static void requireUngrouped(Command... commands) {
    requireUngrouped(Set.of(commands));
  }

  /**
   * Requires that the specified commands not have been already allocated to a CommandGroup. Throws
   * an {@link IllegalArgumentException} if commands have been allocated.
   *
   * @param commands The commands to check
   */
  public static void requireUngrouped(Collection<Command> commands) {
    if (!Collections.disjoint(commands, getGroupedCommands())) {
      throw new IllegalArgumentException("Commands cannot be added to more than one CommandGroup");
    }
  }

  static Set<Command> getGroupedCommands() {
    return m_groupedCommands;
  }

  /**
   * Adds the given commands to the command group.
   *
   * @param commands The commands to add.
   */
  public abstract void addCommands(Command... commands);

  /**
   * Factory method for {@link SequentialCommandGroup}, included for brevity/convenience.
   *
   * @param commands the commands to include
   * @return the command group
   */
  public static CommandGroupBase sequence(Command... commands) {
    return new SequentialCommandGroup(commands);
  }

  /**
   * Factory method for {@link ParallelCommandGroup}, included for brevity/convenience.
   *
   * @param commands the commands to include
   * @return the command group
   */
  public static CommandGroupBase parallel(Command... commands) {
    return new ParallelCommandGroup(commands);
  }

  /**
   * Factory method for {@link ParallelRaceGroup}, included for brevity/convenience.
   *
   * @param commands the commands to include
   * @return the command group
   */
  public static CommandGroupBase race(Command... commands) {
    return new ParallelRaceGroup(commands);
  }

  /**
   * Factory method for {@link ParallelDeadlineGroup}, included for brevity/convenience.
   *
   * @param deadline the deadline command
   * @param commands the commands to include
   * @return the command group
   */
  public static CommandGroupBase deadline(Command deadline, Command... commands) {
    return new ParallelDeadlineGroup(deadline, commands);
  }
}
