// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging.errors;

import edu.wpi.first.epilogue.logging.ClassSpecificLogger;

/** An error handler implementation that prints error information to the console. */
public class ErrorPrinter implements ErrorHandler {
  /** Default constructor. */
  public ErrorPrinter() {}

  @Override
  public void handle(Throwable exception, ClassSpecificLogger<?> logger) {
    System.err.println(
        "[EPILOGUE] An error occurred while logging an instance of "
            + logger.getLoggedType().getName()
            + ": "
            + exception.getMessage());
  }
}
