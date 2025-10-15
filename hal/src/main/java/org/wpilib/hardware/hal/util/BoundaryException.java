// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.util;

/**
 * This exception represents an error in which a lower limit was set as higher than an upper limit.
 */
public class BoundaryException extends RuntimeException {
  /**
   * Create a new exception with the given message.
   *
   * @param message the message to attach to the exception
   */
  public BoundaryException(String message) {
    super(message);
  }

  /**
   * Make sure that the given value is between the upper and lower bounds, and throw an exception if
   * they are not.
   *
   * @param value The value to check.
   * @param lower The minimum acceptable value.
   * @param upper The maximum acceptable value.
   */
  public static void assertWithinBounds(double value, double lower, double upper) {
    if (value < lower || value > upper) {
      throw new BoundaryException(
          "Value must be between " + lower + " and " + upper + ", " + value + " given");
    }
  }

  /**
   * Returns the message for a boundary exception. Used to keep the message consistent across all
   * boundary exceptions.
   *
   * @param value The given value
   * @param lower The lower limit
   * @param upper The upper limit
   * @return the message for a boundary exception
   */
  public static String getMessage(double value, double lower, double upper) {
    return "Value must be between " + lower + " and " + upper + ", " + value + " given";
  }
}
