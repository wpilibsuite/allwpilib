// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

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
   * Adds a tunable double.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public static void add(String name, TunableDouble tunable) {
    m_root.add(name, tunable);
  }

  /**
   * Adds a tunable integer.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public static void add(String name, TunableInt tunable) {
    m_root.add(name, tunable);
  }

  /**
   * Adds a tunable object.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public static void add(String name, TunableObject tunable) {
    m_root.add(name, tunable);
  }

  /**
   * Adds a tunable object.
   *
   * @param <T> data type
   * @param name the name
   * @param tunable the tunable
   */
  public static <T> void add(String name, Tunable<T> tunable) {
    m_root.add(name, tunable);
  }
}
