// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.HashMap;
import java.util.Map;

/** A mock tunable backend for testing. */
public class MockTunableBackend implements TunableBackend {
  private final Map<String, Object> m_entries = new HashMap<>();

  @Override
  public void close() {
    synchronized (m_entries) {
      m_entries.clear();
    }
  }

  @Override
  public void remove(String name) {
    synchronized (m_entries) {
      m_entries.remove(name);
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param name tunable name
   * @param value value
   */
  public void setDouble(String name, double value) {
    synchronized (m_entries) {
      TunableDouble tunable = (TunableDouble) m_entries.get(name);
      if (tunable == null) {
        throw new IllegalArgumentException("No such tunable: " + name);
      }
      tunable.set(value);
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param name tunable name
   * @param value value
   */
  public void setInt(String name, int value) {
    synchronized (m_entries) {
      TunableInt tunable = (TunableInt) m_entries.get(name);
      if (tunable == null) {
        throw new IllegalArgumentException("No such tunable: " + name);
      }
      tunable.set(value);
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param name tunable name
   * @param value value
   */
  public <T> void setValue(String name, T value) {
    synchronized (m_entries) {
      @SuppressWarnings("unchecked")
      Tunable<T> tunable = (Tunable<T>) m_entries.get(name);
      if (tunable == null) {
        throw new IllegalArgumentException("No such tunable: " + name);
      }
      tunable.set(value);
    }
  }

  @Override
  public void addDouble(String name, TunableDouble tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      m_entries.put(name, tunable);
    }
  }

  @Override
  public void addInt(String name, TunableInt tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      m_entries.put(name, tunable);
    }
  }

  @Override
  public void addObject(String name, TunableObject tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      m_entries.put(name, tunable);
    }
  }

  @Override
  public <T> void addTunable(String name, Tunable<T> tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      m_entries.put(name, tunable);
    }
  }

  @Override
  public void update() {}
}
