// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

import java.util.Collections;
import java.util.Set;
import java.util.WeakHashMap;

public class SendableSet implements AutoCloseable {
  private Set<SendableTable> m_sendables = Collections.newSetFromMap<SendableTable>(new WeakHashMap<>());

  @Override
  public void close() {
    for (SendableTable table : m_sendables) {
      try {
        table.close();
      } catch (Exception e) {
        // ignored
      }
    }
  }

  public Set<SendableTable> getAll() {
    return m_sendables;
  }

  public void add(SendableTable table) {
    m_sendables.add(table);
  }

  public void remove(SendableTable table) {
    m_sendables.remove(table);
  }
}
