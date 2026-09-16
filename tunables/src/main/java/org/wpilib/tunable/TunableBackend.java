// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.List;

/** Backend interface for publishing and updating tunables. */
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
   * <p>Backend implementations must not throw from this method. Recoverable publication failures
   * should be reported through {@link TunableRegistry#reportWarning(String)} and skipped.
   *
   * @param path normalized path
   * @param tunable the tunable
   * @return true if the tunable was published, false otherwise
   */
  boolean publish(String path, TunableBase tunable);

  /**
   * Publishes a complex tunable.
   *
   * <p>Backend implementations must not throw from this method. Recoverable publication failures
   * should be reported through {@link TunableRegistry#reportWarning(String)} and skipped.
   *
   * @param path normalized path
   * @param tunable the tunable
   * @return true if the tunable was published, false otherwise
   */
  boolean publishComplex(String path, ComplexTunable tunable);

  /**
   * Removes a tunable.
   *
   * <p>Backend implementations must not throw from this method.
   *
   * @param path normalized path
   */
  void remove(String path);

  /**
   * Removes all tunables under a prefix.
   *
   * <p>Backend implementations must not throw from this method. The returned list must describe the
   * tunables actually removed from the backend.
   *
   * @param prefix normalized path prefix
   * @return removed tunables
   */
  List<PublishedTunable> removePrefix(String prefix);

  /**
   * Notifies the backend that a tunable has local changes.
   *
   * <p>Backend implementations must not throw from this method.
   *
   * @param tunable the changed tunable
   */
  default void markDirty(TunableBase tunable) {}

  /**
   * Updates all tunable values and calls callbacks where appropriate.
   *
   * <p>Backend implementations must not throw from this method. Recoverable update failures should
   * be reported through {@link TunableRegistry#reportWarning(String)} and skipped so the robot loop
   * and registry state can continue updating.
   */
  void update();
}
