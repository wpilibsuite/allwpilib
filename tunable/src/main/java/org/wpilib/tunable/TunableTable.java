// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.IntConsumer;
import java.util.function.IntSupplier;

public final class TunableTable {
  private final String m_path;

  /**
   * Constructs a telemetry table.
   *
   * @param path path with trailing "/".
   */
  TunableTable(String path) {
    m_path = path;
  }

  /**
   * Gets the table path.
   *
   * @return path with trailing "/"
   */
  public String getPath() {
    return m_path;
  }

  /**
   * Gets a child tunable table.
   *
   * @param name table name
   * @return table
   */
  public TunableTable getTable(String name) {
    return TunableRegistry.getTable(m_path + name + "/");
  }

  /**
   * Adds a tunable double.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public void add(String name, TunableDouble tunable) {
    String path = TunableRegistry.normalizeName(m_path + name);
    TunableRegistry.getBackend(path).addDouble(path, tunable);
  }

  public TunableDouble add(String name, DoubleSupplier getter, DoubleConsumer setter) {
    TunableDouble tunable = TunableDouble.create(getter, setter);
    add(name, tunable);
    return tunable;
  }

  /**
   * Adds a tunable integer.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public void add(String name, TunableInt tunable) {
    String path = TunableRegistry.normalizeName(m_path + name);
    TunableRegistry.getBackend(path).addInt(path, tunable);
  }

  public TunableInt add(String name, IntSupplier getter, IntConsumer setter) {
    TunableInt tunable = TunableInt.create(getter, setter);
    add(name, tunable);
    return tunable;
  }

  public void add(String name, TunableObject tunable) {
    String path = TunableRegistry.normalizeName(m_path + name);
    TunableRegistry.getBackend(path).addObject(path, getTable(name), tunable);
  }

  public <T> void add(String name, Tunable<T> tunable) {
    String path = TunableRegistry.normalizeName(m_path + name);
    TunableRegistry.getBackend(path).addTunable(path, tunable);
  }
}
