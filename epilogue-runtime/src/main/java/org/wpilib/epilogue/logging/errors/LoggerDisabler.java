// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging.errors;

import edu.wpi.first.epilogue.logging.ClassSpecificLogger;
import java.util.HashMap;
import java.util.Map;

/**
 * An error handler that disables loggers after too many exceptions are raised. Useful when playing
 * in matches, where data logging is less important than reliable control. Setting the threshold to
 * ≤0 will cause any logger that encounters an exception whilst logging to immediately be disabled.
 * Setting to higher values means your program is more tolerant of errors, but takes longer to
 * disable, and therefore may have more sets of partial or incomplete data and may have more CPU
 * overhead due to the cost of throwing exceptions.
 */
public class LoggerDisabler implements ErrorHandler {
  private final int m_threshold;
  private final Map<ClassSpecificLogger<?>, Integer> m_errorCounts = new HashMap<>();

  /**
   * Creates a new logger-disabling error handler.
   *
   * @param threshold how many errors any one logger is allowed to encounter before it is disabled.
   */
  public LoggerDisabler(int threshold) {
    this.m_threshold = threshold;
  }

  /**
   * Creates a disabler that kicks in after a logger raises more than {@code threshold} exceptions.
   *
   * @param threshold the threshold value for the maximum number of exceptions loggers are permitted
   *     to encounter before they are disabled
   * @return the disabler
   */
  public static LoggerDisabler forLimit(int threshold) {
    return new LoggerDisabler(threshold);
  }

  @Override
  public void handle(Throwable exception, ClassSpecificLogger<?> logger) {
    var errorCount = m_errorCounts.getOrDefault(logger, 0) + 1;
    m_errorCounts.put(logger, errorCount);

    if (errorCount > m_threshold) {
      logger.disable();
      System.err.println(
          "[EPILOGUE] Too many errors detected in "
              + logger.getClass().getName()
              + " (maximum allowed: "
              + m_threshold
              + "). The most recent error follows:");
      System.err.println(exception.getMessage());
      exception.printStackTrace(System.err);
    }
  }

  /** Resets all error counts and reenables all loggers. */
  public void reset() {
    for (var logger : m_errorCounts.keySet()) {
      // Safe. This is a no-op on loggers that are already enabled
      logger.reenable();
    }
    m_errorCounts.clear();
  }
}
