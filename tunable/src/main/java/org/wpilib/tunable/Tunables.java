// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

/**
 * Tunables are used to allow values in the robot program to be changed from dashboards or debug
 * tools.
 *
 * <p>For more advanced use cases, use NetworkTables directly.
 */
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
   * Publishes a complex tunable object.
   *
   * @param name the name
   * @param tunable the tunable
   * @return the passed-in tunable
   */
  public static <T extends ComplexTunable> T addComplex(String name, T tunable) {
    publish(name, tunable);
    return tunable;
  }

  /**
   * Creates and publishes a tunable value.
   *
   * @param name the name
   * @param initialValue the initial value
   * @return tunable
   */
  public static <T> Tunable<T> add(String name, T initialValue) {
    Tunable<T> tunable = Tunable.create(initialValue);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Creates and publishes a tunable boolean value.
   *
   * @param name the name
   * @param initialValue the initial value
   * @return tunable
   */
  public static TunableBoolean addBoolean(String name, boolean initialValue) {
    TunableBoolean tunable = TunableBoolean.create(initialValue);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Creates and publishes a tunable integer value.
   *
   * @param name the name
   * @param initialValue the initial value
   * @return tunable
   */
  public static TunableInt addInt(String name, int initialValue) {
    TunableInt tunable = TunableInt.create(initialValue);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Creates and publishes a tunable long value.
   *
   * @param name the name
   * @param initialValue the initial value
   * @return tunable
   */
  public static TunableLong addLong(String name, long initialValue) {
    TunableLong tunable = TunableLong.create(initialValue);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Creates and publishes a tunable float value.
   *
   * @param name the name
   * @param initialValue the initial value
   * @return tunable
   */
  public static TunableFloat addFloat(String name, float initialValue) {
    TunableFloat tunable = TunableFloat.create(initialValue);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Creates and publishes a tunable double value.
   *
   * @param name the name
   * @param initialValue the initial value
   * @return tunable
   */
  public static TunableDouble addDouble(String name, double initialValue) {
    TunableDouble tunable = TunableDouble.create(initialValue);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Publishes a tunable.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public static void publish(String name, TunableBase tunable) {
    m_root.publish(name, tunable);
  }

  /**
   * Publishes a complex tunable.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public static void publish(String name, ComplexTunable tunable) {
    m_root.publish(name, tunable);
  }

  /**
   * Removes a tunable.
   *
   * @param name the name
   */
  public static void remove(String name) {
    m_root.remove(name);
  }
}
