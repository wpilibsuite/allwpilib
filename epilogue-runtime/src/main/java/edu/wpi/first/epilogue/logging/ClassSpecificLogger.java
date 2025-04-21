// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.epilogue.CustomLoggerFor;
import edu.wpi.first.epilogue.logging.errors.ErrorHandler;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * Base class for class-specific generated loggers. Loggers are generated at compile time by the
 * Epilogue annotation processor and are used at runtime for zero-overhead data logging. Users may
 * also declare custom loggers, annotated with {@link CustomLoggerFor @CustomLoggerFor}, for
 * Epilogue to pull in during compile time to use for logging third party types.
 *
 * @param <T> the type of data supported by the logger
 */
@SuppressWarnings("unused") // Used by generated subclasses
public abstract class ClassSpecificLogger<T> {
  private final Class<T> m_clazz;
  // TODO: This will hold onto Sendables that are otherwise no longer referenced by a robot program.
  //       Determine if that's a concern
  // Linked hashmap to maintain insert order
  private final Map<Sendable, SendableBuilder> m_sendables = new LinkedHashMap<>();

  @SuppressWarnings("PMD.RedundantFieldInitializer")
  private boolean m_disabled = false;

  /**
   * Instantiates the logger.
   *
   * @param clazz the Java class of objects that can be logged
   */
  protected ClassSpecificLogger(Class<T> clazz) {
    this.m_clazz = clazz;
  }

  /**
   * Updates an object's fields in a data log.
   *
   * @param backend the backend to update
   * @param object the object to update in the log
   */
  protected abstract void update(EpilogueBackend backend, T object);

  /**
   * Attempts to update the data log. Will do nothing if the logger is {@link #disable() disabled}.
   *
   * @param backend the backend to log data to
   * @param object the data object to log
   * @param errorHandler the handler to use if logging raised an exception
   */
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  public final void tryUpdate(EpilogueBackend backend, T object, ErrorHandler errorHandler) {
    if (m_disabled) {
      return;
    }

    try {
      update(backend, object);
    } catch (Exception e) {
      errorHandler.handle(e, this);
    }
  }

  /**
   * Checks if this logger has been disabled.
   *
   * @return true if this logger has been disabled by {@link #disable()}, false if not
   */
  public final boolean isDisabled() {
    return m_disabled;
  }

  /** Disables this logger. Any log calls made while disabled will be ignored. */
  public final void disable() {
    m_disabled = true;
  }

  /** Reenables this logger after being disabled. Has no effect if the logger is already enabled. */
  public final void reenable() {
    m_disabled = false;
  }

  /**
   * Gets the type of the data this logger accepts.
   *
   * @return the logged data type
   */
  public final Class<T> getLoggedType() {
    return m_clazz;
  }

  /**
   * Logs a sendable type.
   *
   * @param backend the backend to log data into
   * @param sendable the sendable object to log
   */
  protected void logSendable(EpilogueBackend backend, Sendable sendable) {
    if (sendable == null) {
      return;
    }

    var builder =
        m_sendables.computeIfAbsent(
            sendable,
            s -> {
              var b = new LogBackedSendableBuilder(backend);
              s.initSendable(b);
              return b;
            });
    builder.update();
  }
}
