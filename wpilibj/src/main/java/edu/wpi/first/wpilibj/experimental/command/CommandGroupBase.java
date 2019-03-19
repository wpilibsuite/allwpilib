package edu.wpi.first.wpilibj.experimental.command;

import java.util.Collections;
import java.util.Set;
import java.util.WeakHashMap;

/**
 * A base for CommandGroups.  Statically tracks commands that have been allocated to groups
 * to ensure those commands are not also used independently, which can result in inconsistent
 * command state and unpredictable execution.
 */
public abstract class CommandGroupBase extends SendableCommandBase implements Command {

  private static Set<Command> m_groupedCommands = Collections.newSetFromMap(new WeakHashMap<>());

  static void registerGroupedCommands(Command... commands) {
    m_groupedCommands.addAll(Set.of(commands));
  }

  /**
   * Clears the list of grouped commands, allowing all commands to be freely used again.
   *
   * <p>WARNING: Using this haphazardly can result in unexpected/undesirable behavior.  Do not
   * use this unless you fully understand what you are doing.
   */
  public static void clearGroupedCommands() {
    m_groupedCommands.clear();
  }

  /**
   * Removes a single command from the list of grouped commands, allowing it to be freely
   * used again.
   *
   * <p>WARNING: Using this haphazardly can result in unexpected/undesirable behavior.  Do not
   * use this unless you fully understand what you are doing.
   *
   * @param command the command to remove from the list of grouped commands
   */
  public static void clearGroupedCommand(Command command) {
    m_groupedCommands.remove(command);
  }

  static Set<Command> getGroupedCommands() {
    return m_groupedCommands;
  }

  public abstract void addCommands(Command... commands);
}

