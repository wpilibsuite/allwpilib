// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

public interface TunableBackend extends AutoCloseable {
  /**
   * Publishes a tunable.
   *
   * @param path normalized path
   * @param tunable the tunable
   */
  void publish(String path, TunableBase tunable);

  /**
   * Publishes a complex tunable.
   *
   * @param path normalized path
   * @param tunable the tunable
   */
  void publishComplex(String path, ComplexTunable tunable);

  /**
   * Removes a tunable.
   *
   * @param path normalized path
   */
  void remove(String path);

  /** Updates all tunable values and calls callbacks where appropriate. */
  void update();
}
