// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

import java.util.Collections;
import java.util.Set;
import java.util.WeakHashMap;
import java.util.function.Consumer;

public class SendableSet implements AutoCloseable {
  private final Set<SendableTable> m_tables =
      Collections.newSetFromMap(new WeakHashMap<SendableTable, Boolean>());

  @Override
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  public void close() {
    for (SendableTable table : m_tables) {
      try {
        table.close();
      } catch (Exception e) {
        // ignored
      }
    }
  }

  public void forEach(Consumer<SendableTable> action) {
    for (SendableTable table : m_tables) {
      if (!table.isClosed()) {
        action.accept(table);
      }
    }
  }

  public Iterable<SendableTable> getAll() {
    return m_tables;
  }

  public void add(SendableTable table) {
    m_tables.add(table);
  }

  public void remove(SendableTable table) {
    m_tables.remove(table);
  }
}
