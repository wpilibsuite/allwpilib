package edu.wpi.first.wpilibj.experimental.command;

import java.util.Collections;
import java.util.Set;
import java.util.WeakHashMap;

abstract class CommandGroupBase {

  private static Set<Command> m_groupedCommands = Collections.newSetFromMap(new WeakHashMap<>());

  static void registerGroupedCommands(Command... commands) {
    m_groupedCommands.addAll(Set.of(commands));
  }

  static void clearGroupedCommands() {
    m_groupedCommands.clear();
  }

  static Set<Command> getGroupedCommands() {
    return m_groupedCommands;
  }

  public abstract void addCommands(Command... commands);
}

