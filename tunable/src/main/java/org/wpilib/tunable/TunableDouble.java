// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

/** A tunable integer. */
public interface TunableDouble extends DoubleSupplier, DoubleConsumer {
  static TunableDouble create() {
    return create(0.0);
  }

  static TunableDouble create(double initialValue) {
    return new TunableDouble() {
      @Override
      public void setDefault(double value) {
        m_value = value;
      }

      @Override
      public void set(double value) {
        m_value = value;
        if (m_onChange != null) {
          m_onChange.accept(value);
        }
      }

      @Override
      public double get() {
        return m_value;
      }

      @Override
      public void onChange(DoubleConsumer callback) {
        m_onChange = callback;
      }

      private double m_value = initialValue;
      private DoubleConsumer m_onChange;
    };
  }

  static TunableDouble create(DoubleSupplier getter, DoubleConsumer setter) {
    return new TunableDouble() {
      @Override
      public void setDefault(double value) {
        setter.accept(value);
      }

      @Override
      public void set(double value) {
        setter.accept(value);
        if (m_onChange != null) {
          m_onChange.accept(value);
        }
      }

      @Override
      public double get() {
        return getter.getAsDouble();
      }

      @Override
      public void onChange(DoubleConsumer callback) {
        m_onChange = callback;
      }

      private DoubleConsumer m_onChange;
    };
  }

  /**
   * Sets the default value. A default value will never be used in preference to a set value.
   *
   * @param value value
   */
  void setDefault(double value);

  /**
   * Sets the value. The onChange() callback will be called if the value changed, but not
   * immediately.
   *
   * @param value value
   */
  void set(double value);

  /**
   * Gets the value. Returns the default value if no other value has been set.
   *
   * @return value
   */
  double get();

  /**
   * Sets a callback that will be called when the value changes. Any existing callback is replaced.
   * The callback is called by TunableRegistry.update(); this is typically called by
   * RobotBase.periodic() such that callbacks are called synchronously to the main periodic loop.
   * Callbacks should be designed to run as quickly as possible and not block.
   *
   * @param callback callback function
   */
  void onChange(DoubleConsumer callback);

  @Override
  default double getAsDouble() {
    return get();
  }

  @Override
  default void accept(double value) {
    set(value);
  }
}
