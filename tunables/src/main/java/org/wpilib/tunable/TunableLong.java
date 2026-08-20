// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.LongConsumer;
import java.util.function.LongSupplier;

/** A tunable long. */
public abstract class TunableLong extends TunableBase implements LongSupplier, LongConsumer {
  /**
   * Creates a tunable long with the initial value of 0.
   *
   * @return a tunable long
   */
  public static TunableLong create() {
    return create(0);
  }

  /**
   * Creates a tunable long with the given initial value.
   *
   * @param initialValue initial value
   * @return a tunable long
   */
  public static TunableLong create(long initialValue) {
    return createConfig(initialValue, null);
  }

  /**
   * Creates a tunable long with the given getter and setter.
   *
   * <p>The getter and setter must not throw.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @return a tunable long
   */
  public static TunableLong create(LongSupplier getter, LongConsumer setter) {
    return createConfig(getter, setter, null);
  }

  /**
   * Creates a tunable long with the given config and an initial value of 0.
   *
   * @param config tunable config
   * @return a tunable long
   */
  public static TunableLong createConfig(TunableConfig config) {
    return createConfig(0, config);
  }

  /**
   * Creates a tunable long with the given initial value and config.
   *
   * @param initialValue initial value
   * @param config tunable config
   * @return a tunable long
   */
  public static TunableLong createConfig(long initialValue, TunableConfig config) {
    return new TunableLong(config, true) {
      @Override
      public void set(long value) {
        if (m_value == value) {
          return;
        }
        m_value = value;
        markChanged();
      }

      @Override
      public long get() {
        return m_value;
      }

      private long m_value = initialValue;
    };
  }

  /**
   * Creates a tunable long with the given getter, setter, and config.
   *
   * <p>The getter and setter must not throw.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @param config tunable config
   * @return a tunable long
   */
  public static TunableLong createConfig(
      LongSupplier getter, LongConsumer setter, TunableConfig config) {
    if (config == null) {
      config = TunableConfig.of(TunableOption.ALWAYS_GET);
    } else if (config.getPolling() == TunableConfig.Polling.DEFAULT) {
      config = config.withPolling(TunableConfig.Polling.ALWAYS_GET);
    }
    return new TunableLong(config, true) {
      @Override
      public void set(long value) {
        boolean changed = getter.getAsLong() != value;
        if (setter != null) {
          setter.accept(value);
        }
        if (changed) {
          markChanged();
        }
      }

      @Override
      public long get() {
        return getter.getAsLong();
      }
    };
  }

  /**
   * Constructs a tunable long.
   *
   * @param config tunable config
   */
  protected TunableLong(TunableConfig config) {
    super(config);
  }

  /**
   * Constructs a tunable long.
   *
   * @param config tunable config
   * @param supportsChangeNotification whether the tunable notifies backends when it changes
   */
  protected TunableLong(TunableConfig config, boolean supportsChangeNotification) {
    super(config, supportsChangeNotification);
  }

  /**
   * Sets the value.
   *
   * <p>Implementations must not throw.
   *
   * @param value value
   */
  public abstract void set(long value);

  /**
   * Gets the value.
   *
   * <p>Implementations must not throw.
   *
   * @return value
   */
  public abstract long get();

  @Override
  public long getAsLong() {
    return get();
  }

  @Override
  public void accept(long value) {
    set(value);
  }
}
