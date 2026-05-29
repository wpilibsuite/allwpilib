// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.IntConsumer;
import java.util.function.IntSupplier;

/** A tunable integer. */
public abstract class TunableInt extends TunableBase implements IntSupplier, IntConsumer {
  /**
   * Creates a tunable integer with the initial value of 0.
   *
   * @return a tunable integer
   */
  public static TunableInt create() {
    return create(0);
  }

  /**
   * Creates a tunable integer with the given initial value.
   *
   * @param initialValue initial value
   * @return a tunable integer
   */
  public static TunableInt create(int initialValue) {
    return createConfig(initialValue, null);
  }

  /**
   * Creates a tunable integer with the given getter and setter.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @return a tunable integer
   */
  public static TunableInt create(IntSupplier getter, IntConsumer setter) {
    return createConfig(getter, setter, null);
  }

  /**
   * Creates a tunable integer with the given config and an initial value of 0.
   *
   * @param config tunable config
   * @return a tunable integer
   */
  public static TunableInt createConfig(TunableConfig config) {
    return createConfig(0, config);
  }

  /**
   * Creates a tunable integer with the given initial value and config.
   *
   * @param initialValue initial value
   * @param config tunable config
   * @return a tunable integer
   */
  public static TunableInt createConfig(int initialValue, TunableConfig config) {
    return new TunableInt(config) {
      @Override
      public void set(int value) {
        m_value = value;
        m_changed = true;
      }

      @Override
      public int get() {
        return m_value;
      }

      private int m_value = initialValue;
    };
  }

  /**
   * Creates a tunable integer with the given getter, setter, and config.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @param config tunable config
   * @return a tunable integer
   */
  public static TunableInt createConfig(
      IntSupplier getter, IntConsumer setter, TunableConfig config) {
    if (config == null) {
      config = TunableConfig.of(TunableOption.ALWAYS_GET);
    } else {
      config = config.withAlwaysGet(true);
    }
    return new TunableInt(config) {
      @Override
      public void set(int value) {
        if (setter != null) {
          setter.accept(value);
        }
        m_changed = true;
      }

      @Override
      public int get() {
        return getter.getAsInt();
      }
    };
  }

  protected TunableInt(TunableConfig config) {
    super(config);
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  public abstract void set(int value);

  /**
   * Gets the value.
   *
   * @return value
   */
  public abstract int get();

  @Override
  public int getAsInt() {
    return get();
  }

  @Override
  public void accept(int value) {
    set(value);
  }
}
