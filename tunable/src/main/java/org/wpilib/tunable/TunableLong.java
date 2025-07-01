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
    return new TunableLong(config) {
      @Override
      public void set(long value) {
        m_value = value;
        m_changed = true;
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
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @param config tunable config
   * @return a tunable long
   */
  public static TunableLong createConfig(
      LongSupplier getter, LongConsumer setter, TunableConfig config) {
    if (config == null) {
      config = TunableConfig.of(TunableOption.ALWAYS_GET);
    } else {
      config = config.withAlwaysGet(true);
    }
    return new TunableLong(config) {
      @Override
      public void set(long value) {
        if (setter != null) {
          setter.accept(value);
        }
        m_changed = true;
      }

      @Override
      public long get() {
        return getter.getAsLong();
      }
    };
  }

  protected TunableLong(TunableConfig config) {
    super(config);
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  public abstract void set(long value);

  /**
   * Gets the value.
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
