// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import org.wpilib.util.function.FloatConsumer;
import org.wpilib.util.function.FloatSupplier;

/** A tunable float. */
public abstract class TunableFloat extends TunableBase implements FloatSupplier, FloatConsumer {
  /**
   * Creates a tunable float with the initial value of 0.0.
   *
   * @return a tunable float
   */
  public static TunableFloat create() {
    return create(0.0f);
  }

  /**
   * Creates a tunable float with the given initial value.
   *
   * @param initialValue initial value
   * @return a tunable float
   */
  public static TunableFloat create(float initialValue) {
    return createConfig(initialValue, null);
  }

  /**
   * Creates a tunable float with the given getter and setter.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @return a tunable float
   */
  public static TunableFloat create(FloatSupplier getter, FloatConsumer setter) {
    return createConfig(getter, setter, null);
  }

  /**
   * Creates a tunable float with the given config and an initial value of 0.0.
   *
   * @param config tunable config
   * @return a tunable float
   */
  public static TunableFloat createConfig(TunableConfig config) {
    return createConfig(0.0f, config);
  }

  /**
   * Creates a tunable float with the given initial value and config.
   *
   * @param initialValue initial value
   * @param config tunable config
   * @return a tunable float
   */
  public static TunableFloat createConfig(float initialValue, TunableConfig config) {
    return new TunableFloat(config) {
      @Override
      public void set(float value) {
        m_value = value;
        m_changed = true;
      }

      @Override
      public float get() {
        return m_value;
      }

      private float m_value = initialValue;
    };
  }

  /**
   * Creates a tunable float with the given getter, setter, and config.
   *
   * @param getter getter for the tunable value
   * @param setter setter for the tunable value
   * @param config tunable config
   * @return a tunable float
   */
  public static TunableFloat createConfig(
      FloatSupplier getter, FloatConsumer setter, TunableConfig config) {
    if (config == null) {
      config = TunableConfig.of(TunableOption.ALWAYS_GET);
    } else {
      config = config.withAlwaysGet(true);
    }
    return new TunableFloat(config) {
      @Override
      public void set(float value) {
        if (setter != null) {
          setter.accept(value);
        }
        m_changed = true;
      }

      @Override
      public float get() {
        return getter.getAsFloat();
      }
    };
  }

  protected TunableFloat(TunableConfig config) {
    super(config);
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  public abstract void set(float value);

  /**
   * Gets the value.
   *
   * @return value
   */
  public abstract float get();

  @Override
  public float getAsFloat() {
    return get();
  }

  @Override
  public void accept(float value) {
    set(value);
  }
}
