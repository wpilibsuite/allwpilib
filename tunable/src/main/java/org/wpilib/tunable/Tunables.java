// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

public final class Tunables {
  /** The root {@link TunableTable}. */
  private static final TunableTable m_root = TunableRegistry.getTable("/");

  private Tunables() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Gets the root tunable table.
   *
   * @return table
   */
  public static TunableTable getTable() {
    return m_root;
  }

  /**
   * Gets a child tunable table.
   *
   * @param name table name
   * @return table
   */
  public static TunableTable getTable(String name) {
    return m_root.getTable(name);
  }

  /**
   * Adds a tunable integer.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value
   * @return Tunable
   */
  public static TunableInt add(String name, int defaultValue) {
    return m_root.add(name, defaultValue);
  }

  /**
   * Adds a tunable object.
   *
   * @param <T> data type
   * @param name the name
   * @param defaultValue the default value (may be null)
   * @return Tunable
   */
  public static <T> Tunable<T> add(String name, T defaultValue) {
    return m_root.add(name, defaultValue);
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
  public static <T> Tunable<T> add(String name, T defaultValue, Struct<T> struct) {
    return m_root.add(name, defaultValue, struct);
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
  public static <T> Tunable<T> add(String name, T defaultValue, Protobuf<T, ?> proto) {
    return m_root.add(name, defaultValue, proto);
  }
}
