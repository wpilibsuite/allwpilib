// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging.errors;

import edu.wpi.first.epilogue.logging.ClassSpecificLogger;

/**
 * An error handler implementation that will throw an exception if logging raised an exception. This
 * is useful when running code in simulation or in JUnit tests to quickly identify errors in your
 * code.
 */
public class CrashOnError implements ErrorHandler {
  /** Default constructor. */
  public CrashOnError() {}

  @Override
  public void handle(Throwable exception, ClassSpecificLogger<?> logger) {
    throw new RuntimeException(
        "[EPILOGUE] An error occurred while logging an instance of "
            + logger.getLoggedType().getName(),
        exception);
  }
}
