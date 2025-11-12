// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.Consumer;
import java.util.function.Supplier;

/**
 * A tunable complex value.
 *
 * @param <T> value class
 */
public interface Tunable<T> extends Supplier<T>, Consumer<T> {
  /**
   * Sets the default value. A default value will never be used in preference to a
   * set value.
   *
   * @param value value
   */
  void setDefault(T value);

  /**
   * Sets the value. The onChange() callback will be called if the value changed, but not immediately.
   *
   * @param value value
   */
  void set(T value);

  /**
   * Gets the value. Returns the default value if no other value has been set.
   *
   * @return value
   */
  @Override
  T get();

  /**
   * Sets a callback that will be called when the value changes. Any existing callback is replaced.
   * The callback is called by TunableRegistry.update(); this is typically called by RobotBase.periodic()
   * such that callbacks are called synchronously to the main periodic loop. Callbacks should be designed
   * to run as quickly as possible and not block.
   *
   * @param callback callback function
   */
  void onChange(Consumer<T> callback);

  @Override
  default void accept(T value) {
    set(value);
  }
}
