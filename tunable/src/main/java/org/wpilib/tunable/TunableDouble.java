// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

/** A tunable double. */
public interface TunableDouble extends DoubleSupplier, DoubleConsumer {
  static TunableDouble create() {
    return create(0.0);
  }

  static TunableDouble create(double initialValue) {
    return new TunableDouble() {
      @Override
      public void set(double value) {
        m_value = value;
      }

      @Override
      public double get() {
        return m_value;
      }

      private double m_value = initialValue;
    };
  }

  static TunableDouble create(DoubleSupplier getter, DoubleConsumer setter) {
    return new TunableDouble() {
      @Override
      public void set(double value) {
        setter.accept(value);
      }

      @Override
      public double get() {
        return getter.getAsDouble();
      }
    };
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  void set(double value);

  /**
   * Gets the value.
   *
   * @return value
   */
  double get();

  @Override
  default double getAsDouble() {
    return get();
  }

  @Override
  default void accept(double value) {
    set(value);
  }
}
