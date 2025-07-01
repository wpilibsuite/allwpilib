// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

/** A tunable double. */
public abstract class TunableDouble extends TunableBase implements DoubleSupplier, DoubleConsumer {
  /**
   * Creates a tunable double with the initial value of 0.0.
   *
   * @return a tunable double
   */
  public static TunableDouble create() {
    return create(0.0);
  }

  /**
   * Creates a tunable double with the given initial value.
   *
   * @param initialValue initial value
   * @return a tunable double
   */
  public static TunableDouble create(double initialValue) {
    return createConfig(initialValue, null);
  }

  /**
   * Creates a tunable double with the given getter and setter.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @return a tunable double
   */
  public static TunableDouble create(DoubleSupplier getter, DoubleConsumer setter) {
    return createConfig(getter, setter, null);
  }

  /**
   * Creates a tunable double with the given config and an initial value of 0.0.
   *
   * @param config tunable config
   * @return a tunable double
   */
  public static TunableDouble createConfig(TunableConfig config) {
    return createConfig(0.0, config);
  }

  /**
   * Creates a tunable double with the given initial value and config.
   *
   * @param initialValue initial value
   * @param config tunable config
   * @return a tunable double
   */
  public static TunableDouble createConfig(double initialValue, TunableConfig config) {
    return new TunableDouble(config) {
      @Override
      public void set(double value) {
        m_value = value;
        m_changed = true;
      }

      @Override
      public double get() {
        return m_value;
      }

      private double m_value = initialValue;
    };
  }

  /**
   * Creates a tunable double with the given getter, setter, and config.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @param config tunable config
   * @return a tunable double
   */
  public static TunableDouble createConfig(
      DoubleSupplier getter, DoubleConsumer setter, TunableConfig config) {
    if (config == null) {
      config = TunableConfig.of(TunableOption.ALWAYS_GET);
    } else {
      config = config.withAlwaysGet(true);
    }
    return new TunableDouble(config) {
      @Override
      public void set(double value) {
        if (setter != null) {
          setter.accept(value);
        }
        m_changed = true;
      }

      @Override
      public double get() {
        return getter.getAsDouble();
      }
    };
  }

  protected TunableDouble(TunableConfig config) {
    super(config);
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  public abstract void set(double value);

  /**
   * Gets the value.
   *
   * @return value
   */
  public abstract double get();

  @Override
  public double getAsDouble() {
    return get();
  }

  @Override
  public void accept(double value) {
    set(value);
  }
}
