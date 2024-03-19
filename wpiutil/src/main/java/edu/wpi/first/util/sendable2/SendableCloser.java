// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

import java.util.Map;
import java.util.WeakHashMap;

public class SendableCloser implements AutoCloseable {
  private Map<SendableTable, Boolean> m_sendables = new WeakHashMap<>();

  @Override
  public void close() {
    for (SendableTable table : m_sendables.keySet()) {
      try {
        table.close();
      } catch (Exception e) {
        // ignored
      }
    }
  }

  public void add(SendableTable table) {
    m_sendables.put(table, null);
  }

  public void remove(SendableTable table) {
    m_sendables.remove(table);
  }
}
