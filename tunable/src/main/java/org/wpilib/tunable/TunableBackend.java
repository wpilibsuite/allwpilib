// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

public interface TunableBackend extends AutoCloseable {
  /**
   * Removes a tunable.
   *
   * @param name name
   */
  void remove(String name);

  /**
   * Adds a tunable double.
   *
   * @param name the name
   * @param tunable the tunable
   */
  void addDouble(String name, TunableDouble tunable);

  /**
   * Adds a tunable integer.
   *
   * @param name the name
   * @param tunable the tunable
   */
  void addInt(String name, TunableInt tunable);

  /**
   * Adds a tunable object.
   *
   * @param name the name
   * @param tunable the tunable
   */
  void addObject(String name, TunableObject tunable);

  /**
   * Adds a tunable-wrapped object.
   *
   * @param <T> data type
   * @param name the name
   * @param tunable the tunable
   */
  <T> void addTunable(String name, Tunable<T> tunable);

  /** Updates all tunable values and calls callbacks where appropriate. */
  void update();
}
