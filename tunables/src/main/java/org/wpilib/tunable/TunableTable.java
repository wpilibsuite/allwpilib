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
import org.wpilib.tunable.util.PathUtil;
import org.wpilib.util.function.BooleanConsumer;
import org.wpilib.util.function.FloatConsumer;
import org.wpilib.util.function.FloatSupplier;

/**
 * Tunables are used to allow values in the robot program to be changed from dashboards or debug
 * tools.
 *
 * <p>Getter/setter-backed publish methods take getters and setters that must not throw.
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
   * @return true if the backend accepted the tunable
   */
  public boolean publish(String name, TunableBase tunable) {
    return TunableRegistry.publish(PathUtil.normalizeName(m_path + name), tunable);
  }

  /**
   * Publishes a complex tunable.
   *
   * @param name the name
   * @param tunable the tunable
   * @return true if the backend accepted the tunable
   */
  public boolean publish(String name, ComplexTunable tunable) {
    return TunableRegistry.publish(PathUtil.normalizeName(m_path + name), tunable);
  }

  /**
   * Publishes a tunable value using a getter and setter.
   *
   * @param <T> value type
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @param cls the class of the value
   * @return the tunable
   */
  public <T> Tunable<T> publishValue(
      String name, Supplier<T> getter, Consumer<T> setter, Class<T> cls) {
    return publishValue(name, getter, setter, cls, null);
  }

  /**
   * Publishes a tunable value using a getter and setter.
   *
   * @param <T> value type
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @param cls the class of the value
   * @param config tunable config
   * @return the tunable
   */
  public <T> Tunable<T> publishValue(
      String name, Supplier<T> getter, Consumer<T> setter, Class<T> cls, TunableConfig config) {
    Tunable<T> tunable = Tunable.createConfig(getter, setter, cls, config);
    publish(name, tunable);
    return tunable;
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
    return publishBoolean(name, getter, setter, null);
  }

  /**
   * Publishes a tunable boolean value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @param config tunable config
   * @return the tunable
   */
  public TunableBoolean publishBoolean(
      String name, BooleanSupplier getter, BooleanConsumer setter, TunableConfig config) {
    TunableBoolean tunable = TunableBoolean.createConfig(getter, setter, config);
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
    return publishInt(name, getter, setter, null);
  }

  /**
   * Publishes a tunable integer value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @param config tunable config
   * @return the tunable
   */
  public TunableInt publishInt(
      String name, IntSupplier getter, IntConsumer setter, TunableConfig config) {
    TunableInt tunable = TunableInt.createConfig(getter, setter, config);
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
    return publishLong(name, getter, setter, null);
  }

  /**
   * Publishes a tunable long value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @param config tunable config
   * @return the tunable
   */
  public TunableLong publishLong(
      String name, LongSupplier getter, LongConsumer setter, TunableConfig config) {
    TunableLong tunable = TunableLong.createConfig(getter, setter, config);
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
    return publishFloat(name, getter, setter, null);
  }

  /**
   * Publishes a tunable float value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @param config tunable config
   * @return the tunable
   */
  public TunableFloat publishFloat(
      String name, FloatSupplier getter, FloatConsumer setter, TunableConfig config) {
    TunableFloat tunable = TunableFloat.createConfig(getter, setter, config);
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
    return publishDouble(name, getter, setter, null);
  }

  /**
   * Publishes a tunable double value using a getter and setter.
   *
   * @param name the name
   * @param getter the getter
   * @param setter the setter
   * @param config tunable config
   * @return the tunable
   */
  public TunableDouble publishDouble(
      String name, DoubleSupplier getter, DoubleConsumer setter, TunableConfig config) {
    TunableDouble tunable = TunableDouble.createConfig(getter, setter, config);
    publish(name, tunable);
    return tunable;
  }

  /**
   * Removes a tunable.
   *
   * @param name name
   */
  public void remove(String name) {
    TunableRegistry.remove(PathUtil.normalizeName(m_path + name));
  }
}
