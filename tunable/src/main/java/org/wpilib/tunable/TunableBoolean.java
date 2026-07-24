// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.BooleanSupplier;
import org.wpilib.util.function.BooleanConsumer;

/** A tunable boolean. */
public abstract class TunableBoolean extends TunableBase
    implements BooleanSupplier, BooleanConsumer {
  /**
   * Creates a tunable boolean with the initial value of false.
   *
   * @return a tunable boolean
   */
  public static TunableBoolean create() {
    return create(false);
  }

  /**
   * Creates a tunable boolean with the given initial value.
   *
   * @param initialValue initial value
   * @return a tunable boolean
   */
  public static TunableBoolean create(boolean initialValue) {
    return createConfig(initialValue, null);
  }

  /**
   * Creates a tunable boolean with the given getter and setter.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @return a tunable boolean
   */
  public static TunableBoolean create(BooleanSupplier getter, BooleanConsumer setter) {
    return createConfig(getter, setter, null);
  }

  /**
   * Creates a tunable boolean with the given config and an initial value of false.
   *
   * @param config tunable config
   * @return a tunable boolean
   */
  public static TunableBoolean createConfig(TunableConfig config) {
    return createConfig(false, config);
  }

  /**
   * Creates a tunable boolean with the given initial value and config.
   *
   * @param initialValue initial value
   * @param config tunable config
   * @return a tunable boolean
   */
  public static TunableBoolean createConfig(boolean initialValue, TunableConfig config) {
    return new TunableBoolean(config) {
      @Override
      public void set(boolean value) {
        m_value = value;
        m_changed = true;
      }

      @Override
      public boolean get() {
        return m_value;
      }

      private boolean m_value = initialValue;
    };
  }

  /**
   * Creates a tunable boolean with the given getter, setter, and config.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @param config tunable config
   * @return a tunable boolean
   */
  public static TunableBoolean createConfig(
      BooleanSupplier getter, BooleanConsumer setter, TunableConfig config) {
    if (config == null) {
      config = TunableConfig.of(TunableOption.ALWAYS_GET);
    } else {
      config = config.withAlwaysGet(true);
    }
    return new TunableBoolean(config) {
      @Override
      public void set(boolean value) {
        if (setter != null) {
          setter.accept(value);
        }
        m_changed = true;
      }

      @Override
      public boolean get() {
        return getter.getAsBoolean();
      }
    };
  }

  protected TunableBoolean(TunableConfig config) {
    super(config);
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  public abstract void set(boolean value);

  /**
   * Gets the value.
   *
   * @return value
   */
  public abstract boolean get();

  @Override
  public boolean getAsBoolean() {
    return get();
  }

  @Override
  public void accept(boolean value) {
    set(value);
  }
}
