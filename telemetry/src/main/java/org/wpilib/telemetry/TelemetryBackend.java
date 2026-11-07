// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

/**
 * Interface for telemetry backends.
 *
 * <p>Implementations registered with {@link TelemetryRegistry} must be thread-safe. Backend and
 * entry methods may be called concurrently, and entry removal or backend closure may overlap a call
 * already in progress on a previously returned entry.
 */
public interface TelemetryBackend extends AutoCloseable {
  /**
   * Create an entry for the given path.
   *
   * <p>Backend implementations must not throw from this method. Recoverable failures should be
   * reported through {@link TelemetryRegistry#reportWarning(String, String)} and represented with a
   * discard entry.
   *
   * @param path full name
   * @return telemetry entry
   */
  TelemetryEntry getEntry(String path);

  /**
   * Returns whether this backend is or owns the given backend instance for lifecycle purposes.
   *
   * <p>Composite backends should override this and recursively check child backends that are closed
   * when this backend is closed. The registry uses this to avoid closing a displaced backend that
   * is now retained by another registered backend.
   *
   * @param backend backend instance
   * @return True if this backend is or owns the given backend
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  default boolean ownsBackend(TelemetryBackend backend) {
    return this == backend;
  }

  /**
   * Returns whether this backend shares lifecycle ownership with another backend.
   *
   * <p>The registry uses this to avoid closing displaced backends whose close operation would close
   * a backend that remains registered elsewhere.
   *
   * @param backend backend instance
   * @return True if this backend owns, is owned by, or shares a child backend with the given
   *     backend
   */
  default boolean sharesBackendWith(TelemetryBackend backend) {
    return ownsBackend(backend) || backend.ownsBackend(this);
  }

  /**
   * Removes an entry for the given path.
   *
   * <p>Backend implementations must not throw from this method.
   *
   * @param path normalized full name
   */
  default void removeEntry(String path) {}
}
