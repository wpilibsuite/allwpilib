// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.IntConsumer;
import java.util.function.IntSupplier;

/** A tunable integer. */
public interface TunableInt extends IntSupplier, IntConsumer {
  static TunableInt create() {
    return create(0);
  }

  static TunableInt create(int initialValue) {
    return new TunableInt() {
      @Override
      public void set(int value) {
        m_value = value;
      }

      @Override
      public int get() {
        return m_value;
      }

      private int m_value = initialValue;
    };
  }

  static TunableInt create(IntSupplier getter, IntConsumer setter) {
    return new TunableInt() {
      @Override
      public void set(int value) {
        setter.accept(value);
      }

      @Override
      public int get() {
        return getter.getAsInt();
      }
    };
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  void set(int value);

  /**
   * Gets the value.
   *
   * @return value
   */
  int get();

  @Override
  default int getAsInt() {
    return get();
  }

  @Override
  default void accept(int value) {
    set(value);
  }
}
