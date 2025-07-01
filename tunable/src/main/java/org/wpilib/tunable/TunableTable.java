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
public final class TunableTable {
  private final String m_path;

  /**
   * Constructs a tunable table.
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
   * Publishes a tunable.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public void publish(String name, TunableBase tunable) {
    TunableRegistry.publish(TunableRegistry.normalizeName(m_path + name), tunable);
  }

  /**
   * Publishes a complex tunable.
   *
   * @param name the name
   * @param tunable the tunable
   */
  public void publish(String name, ComplexTunable tunable) {
    TunableRegistry.publish(TunableRegistry.normalizeName(m_path + name), tunable);
  }

  /**
   * Publishes a tunable boolean value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public TunableBoolean publishBoolean(
      String name, BooleanSupplier getter, BooleanConsumer setter) {
    TunableBoolean tunable = TunableBoolean.create(getter, setter);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Publishes a tunable integer value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public TunableInt publishInt(String name, IntSupplier getter, IntConsumer setter) {
    TunableInt tunable = TunableInt.create(getter, setter);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Publishes a tunable long value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public TunableLong publishLong(String name, LongSupplier getter, LongConsumer setter) {
    TunableLong tunable = TunableLong.create(getter, setter);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Publishes a tunable float value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public TunableFloat publishFloat(String name, FloatSupplier getter, FloatConsumer setter) {
    TunableFloat tunable = TunableFloat.create(getter, setter);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Publishes a tunable double value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @return the tunable
   */
  public TunableDouble publishDouble(String name, DoubleSupplier getter, DoubleConsumer setter) {
    TunableDouble tunable = TunableDouble.create(getter, setter);
    publish(name, tunable);
    return tunable;
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
  public <T> Tunable<T> publishValue(
      String name, Supplier<T> getter, Consumer<T> setter, Class<T> cls) {
    Tunable<T> tunable = Tunable.create(getter, setter, cls);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Removes a tunable.
   *
   * @param name name
   */
  public void remove(String name) {
    TunableRegistry.remove(TunableRegistry.normalizeName(m_path + name));
  }
}
