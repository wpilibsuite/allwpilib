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
   * <p>The getter and setter must not throw.
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
    return new TunableBoolean(config, true) {
      @Override
      public void set(boolean value) {
        if (m_value == value) {
          return;
        }
        m_value = value;
        markChanged();
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
   * <p>The getter and setter must not throw.
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
    } else if (config.getPolling() == TunableConfig.Polling.DEFAULT) {
      config = config.withPolling(TunableConfig.Polling.ALWAYS_GET);
    }
    return new TunableBoolean(config, true) {
      @Override
      public void set(boolean value) {
        boolean changed = getter.getAsBoolean() != value;
        if (setter != null) {
          setter.accept(value);
        }
        if (changed) {
          markChanged();
        }
      }

      @Override
      public boolean get() {
        return getter.getAsBoolean();
      }
    };
  }

  /**
   * Constructs a tunable boolean.
   *
   * @param config tunable config
   */
  protected TunableBoolean(TunableConfig config) {
    super(config);
  }

  /**
   * Constructs a tunable boolean.
   *
   * @param config tunable config
   * @param supportsChangeNotification whether the tunable notifies backends when it changes
   */
  protected TunableBoolean(TunableConfig config, boolean supportsChangeNotification) {
    super(config, supportsChangeNotification);
  }

  /**
   * Sets the value.
   *
   * <p>Implementations must not throw.
   *
   * @param value value
   */
  public abstract void set(boolean value);

  /**
   * Gets the value.
   *
   * <p>Implementations must not throw.
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
