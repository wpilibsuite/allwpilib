// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

public final class TunableTable {
  private final ConcurrentMap<String, TunableTable> m_tablesMap = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, TunableEntry> m_entriesMap = new ConcurrentHashMap<>();
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
    m_entriesMap.clear();
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
   * Gets a telemetry entry.
   *
   * @param name name
   * @return entry
   */
  private TunableEntry getEntry(String name) {
    return m_entriesMap.computeIfAbsent(name, k -> TunableRegistry.getEntry(m_path + k));
  }

  public TunableDouble add(String name, TunableDouble tunable) {
    // TODO
    return tunable;
  }

  public TunableDouble add(String name, DoubleSupplier getter, DoubleConsumer setter) {
    // TODO
  }

/*
  public int get(String name, int defaultValue) {
    return getEntry(name).getInt(defaultValue);
  }

  public void set(String name, int value) {
    getEntry(name).setInt(value);
  }
*/
  /**
   * Adds a tunable integer.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value
   * @return Tunable
   */
  public TunableInt add(String name, int defaultValue) {
    String path = TunableRegistry.normalizeName(m_path + name);
    return TunableRegistry.getBackend(path).addInt(path, defaultValue);
  }

  /**
   * Adds a tunable integer.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value
   * @return Tunable
   */
  public TunableDouble add(String name, double defaultValue) {
    String path = TunableRegistry.normalizeName(m_path + name);
    return TunableRegistry.getBackend(path).addDouble(path, defaultValue);
  }

  /**
   * Adds a tunable object.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value (may be null)
   * @return Tunable
   */
  public <T> Tunable<T> add(String name, T defaultValue) {
    String path = TunableRegistry.normalizeName(m_path + name);
    return TunableRegistry.getBackend(path).addObject(path, defaultValue);
  }

  /**
   * Adds a tunable object with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value (may be null)
   * @param struct struct serializer
   * @return Tunable
   */
  public <T> Tunable<T> add(String name, T defaultValue, Struct<T> struct) {
    String path = TunableRegistry.normalizeName(m_path + name);
    return TunableRegistry.getBackend(path).addStruct(path, defaultValue, struct);
  }

  /**
   * Adds a tunable object with a Protobuf serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value (may be null)
   * @param proto protobuf serializer
   * @return Tunable
   */
  public <T> Tunable<T> add(String name, T defaultValue, Protobuf<T, ?> proto) {
    String path = TunableRegistry.normalizeName(m_path + name);
    return TunableRegistry.getBackend(path).addProtobuf(path, defaultValue, proto);
  }
}
