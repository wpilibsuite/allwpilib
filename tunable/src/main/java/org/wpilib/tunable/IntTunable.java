// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.IntConsumer;
import java.util.function.IntSupplier;

/** A tunable integer. */
public interface IntTunable extends IntSupplier, IntConsumer {
  /**
   * Sets the default value. A default value will never be used in preference to a set value.
   *
   * @param value value
   */
  void setDefault(int value);

  /**
   * Sets the value. The onChange() callback will be called if the value changed, but not
   * immediately.
   *
   * @param value value
   */
  void set(int value);

  /**
   * Gets the value. Returns the default value if no other value has been set.
   *
   * @return value
   */
  int get();

  /**
   * Sets a callback that will be called when the value changes. Any existing callback is replaced.
   * The callback is called by TunableRegistry.update(); this is typically called by
   * RobotBase.periodic() such that callbacks are called synchronously to the main periodic loop.
   * Callbacks should be designed to run as quickly as possible and not block.
   *
   * @param callback callback function
   */
  void onChange(IntConsumer callback);

  @Override
  default int getAsInt() {
    return get();
  }

  @Override
  default void accept(int value) {
    set(value);
  }
}
