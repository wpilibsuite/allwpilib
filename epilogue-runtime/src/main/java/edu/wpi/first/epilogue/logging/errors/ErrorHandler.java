// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging.errors;

import edu.wpi.first.epilogue.logging.ClassSpecificLogger;

/**
 * An error handler is used by the Logged framework to catch and process any errors that occur
 * during the logging process. Different handlers can be used in different operating modes, such as
 * crashing in simulation to identify errors before they make it to a robot, or automatically
 * disabling loggers if they encounter too many errors on the field to let the robot keep running
 * while playing a match.
 */
@FunctionalInterface
public interface ErrorHandler {
  /**
   * Handles an exception that arose while logging.
   *
   * @param exception the exception that occurred
   * @param logger the logger that was being processed that caused the error to occur
   */
  void handle(Throwable exception, ClassSpecificLogger<?> logger);

  /**
   * Creates an error handler that will immediately re-throw an exception and cause robot code to
   * exit. This is particularly useful when running in simulation or JUnit tests to identify errors
   * quickly and safely.
   *
   * @return the error handler
   */
  static ErrorHandler crashOnError() {
    return new CrashOnError();
  }

  /**
   * Creates an error handler that will print error messages to the console output, but otherwise
   * allow logging to continue in the future. This can be helpful when errors occur only rarely and
   * you don't want your robot program to crash or disable future logging.
   *
   * @return the error handler
   */
  static ErrorHandler printErrorMessages() {
    return new ErrorPrinter();
  }

  /**
   * Creates an error handler that will automatically disable a logger if it encounters too many
   * errors. Only the error-prone logger(s) will be disabled; loggers that have not encountered any
   * errors, or encountered fewer than the limit, will continue to be used. Disabled loggers can be
   * reset by calling {@link LoggerDisabler#reset()} on the handler.
   *
   * @param maximumPermissibleErrors the maximum number of errors that a logger is permitted to
   *     encounter before being disabled.
   * @return the error handler
   */
  static LoggerDisabler disabling(int maximumPermissibleErrors) {
    return LoggerDisabler.forLimit(maximumPermissibleErrors);
  }
}
