package edu.wpi.first.epilogue.logging.errors;

import edu.wpi.first.epilogue.logging.ClassSpecificLogger;

/** An error handler implementation that prints error information to the console. */
public class ErrorPrinter implements ErrorHandler {
  @Override
  public void handle(Throwable exception, ClassSpecificLogger<?> logger) {
    System.err.println(
        "[EPILOGUE] An error occurred while logging an instance of "
            + logger.getLoggedType().getName()
            + ": "
            + exception.getMessage());
  }
}
