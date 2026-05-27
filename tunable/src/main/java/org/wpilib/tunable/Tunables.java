// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.IntConsumer;
import java.util.function.IntSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;
import org.wpilib.util.function.BooleanConsumer;
import org.wpilib.util.function.FloatConsumer;
import org.wpilib.util.function.FloatSupplier;

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
   * Publishes a tunable boolean value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public static TunableBoolean publishBoolean(
      String name, BooleanSupplier getter, BooleanConsumer setter) {
    return m_root.publishBoolean(name, getter, setter);
  }

  /**
   * Publishes a tunable integer value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public static TunableInt publishInt(String name, IntSupplier getter, IntConsumer setter) {
    return m_root.publishInt(name, getter, setter);
  }

  /**
   * Publishes a tunable long value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public static TunableLong publishLong(String name, LongSupplier getter, LongConsumer setter) {
    return m_root.publishLong(name, getter, setter);
  }

  /**
   * Publishes a tunable float value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public static TunableFloat publishFloat(String name, FloatSupplier getter, FloatConsumer setter) {
    return m_root.publishFloat(name, getter, setter);
  }

  /**
   * Publishes a tunable double value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public static TunableDouble publishDouble(
      String name, DoubleSupplier getter, DoubleConsumer setter) {
    return m_root.publishDouble(name, getter, setter);
  }

  /**
   * Publishes a tunable value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @param cls the class of the value
   * @return the tunable
   */
  public static <T> Tunable<T> publishValue(
      String name, Supplier<T> getter, Consumer<T> setter, Class<T> cls) {
    return m_root.publishValue(name, getter, setter, cls);
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
