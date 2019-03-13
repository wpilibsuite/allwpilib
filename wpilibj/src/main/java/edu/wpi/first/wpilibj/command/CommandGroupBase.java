package edu.wpi.first.wpilibj.command;

import java.util.Collections;
import java.util.Set;
import java.util.WeakHashMap;

abstract class CommandGroupBase {

  private static Set<ICommand> m_groupedCommands = Collections.newSetFromMap(new WeakHashMap<>());

  static void registerGroupedCommands(ICommand... commands) {
    m_groupedCommands.addAll(Set.of(commands));
  }

  static Set<ICommand> getGroupedCommands() {
    return m_groupedCommands;
  }
}

