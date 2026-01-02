// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.IntConsumer;
import java.util.function.IntSupplier;
import java.util.function.Supplier;

public final class TunableTable {
  private final ConcurrentMap<String, TunableTable> m_tablesMap = new ConcurrentHashMap<>();
  private final String m_path;

  /**
   * Constructs a telemetry table.
   *
   * @param path path with trailing "/".
   */
  TunableTable(String path) {
    m_path = path;
  }

  /** Clears the table's cached entries. */
  void reset() {
    m_tablesMap.clear();
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
    return m_tablesMap.computeIfAbsent(name, k -> TunableRegistry.getTable(m_path + k + "/"));
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

  public void add(String name, DoubleSupplier getter, DoubleConsumer setter) {
    add(name, TunableDouble.create(getter, setter));
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

  public void add(String name, IntSupplier getter, IntConsumer setter) {
    add(name, TunableInt.create(getter, setter));
  }

  public void add(String name, TunableObject tunable) {
    String path = TunableRegistry.normalizeName(m_path + name);
    TunableRegistry.getBackend(path).addObject(path, tunable);
  }

  public <T> void add(String name, Tunable<T> tunable) {
    String path = TunableRegistry.normalizeName(m_path + name);
    TunableRegistry.getBackend(path).addTunable(path, tunable);
  }

  public <T> void add(String name, Supplier<T> getter, Consumer<T> setter) {
    add(name, Tunable.create(getter, setter));
  }
}
