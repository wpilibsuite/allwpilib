// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import java.util.HashMap;
import java.util.Map;

import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableBackend;
import org.wpilib.tunable.TunableDouble;
import org.wpilib.tunable.TunableInt;
import org.wpilib.tunable.TunableObject;

public class NetworkTablesTunableBackend implements TunableBackend {
  private final NetworkTableInstance m_inst;
  private final String m_prefix;
  private final Map<String, TunableEntry> m_entries = new HashMap<>();

  private interface TunableEntry extends AutoCloseable {
    void update();
  }

  private class TunableDoubleEntry implements TunableEntry {
    TunableDoubleEntry(String name, TunableDouble tunable) {
      m_tunable = tunable;
    }

    @Override
    public void close() {
      // No-op
    }

    @Override
    public void update() {
      TunableDouble tunable = (TunableDouble) m_entries.get(m_name);
      double ntValue = m_entry.getDouble(tunable.get());
      if (ntValue != tunable.get()) {
        tunable.set(ntValue);
      } else {
        m_entry.setDouble(tunable.get());
      }
    }

    private final TunableDouble m_tunable;
    private final
    private final NetworkTableEntry m_entry =
        m_inst.getEntry(m_prefix + m_name);
  }

  /**
   * Construct.
   *
   * @param inst NetworkTables instance
   * @param prefix prefix to put in front of tunable paths in NT
   */
  public NetworkTablesTunableBackend(NetworkTableInstance inst, String prefix) {
    m_inst = inst;
    m_prefix = prefix;
  }

  @Override
  public void close() {
    synchronized (m_entries) {
      for (Entry entry : m_entries.values()) {
        entry.close();
      }
      m_entries.clear();
    }
  }

  @Override
  public void remove(String name) {
    synchronized (m_entries) {
      Entry entry = m_entries.remove(name);
      if (entry != null) {
        entry.close();
      }
    }
  }

  @Override
  public void addDouble(String name, TunableDouble tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      m_entries.put(name, new TunableDoubleEntry(m_prefix + name, tunable));
      String path = m_prefix + name;

      NetworkTableEntry entry = m_inst.getEntry(path);
      entry.setDouble(tunable.get());
      tunable.set(entry.getDouble(tunable.get()));
      m_entries.put(name, new Entry(entry));
    }
  }

  @Override
  public void addInt(String name, TunableInt tunable) {
  }

  @Override
  public void addObject(String name, TunableObject tunable) {
  }

  @Override
  public <T> void addTunable(String name, Tunable<T> tunable) {
  }

  @Override
  public void update() {
  }
}
