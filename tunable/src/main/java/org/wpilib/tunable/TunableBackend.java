// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.List;

public interface TunableBackend extends AutoCloseable {
  /**
   * A tunable removed from a backend.
   *
   * @param path normalized path
   * @param tunable simple tunable; null for complex tunables
   * @param complex complex tunable; null for simple tunables
   */
  record PublishedTunable(String path, TunableBase tunable, ComplexTunable complex) {
    boolean isComplex() {
      return complex != null;
    }
  }

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

  /**
   * Removes all tunables under a prefix.
   *
   * @param prefix normalized path prefix
   * @return removed tunables
   */
  List<PublishedTunable> removePrefix(String prefix);

  /** Updates all tunable values and calls callbacks where appropriate. */
  void update();
}
