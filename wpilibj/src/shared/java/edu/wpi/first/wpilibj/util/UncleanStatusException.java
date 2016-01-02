/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

/**
 * Exception for bad status codes from the chip object
 *$
 * @author Brian
 */
public final class UncleanStatusException extends IllegalStateException {

  private final int statusCode;

  /**
   * Create a new UncleanStatusException
   *$
   * @param status the status code that caused the exception
   * @param message A message describing the exception
   */
  public UncleanStatusException(int status, String message) {
    super(message);
    statusCode = status;
  }

  /**
   * Create a new UncleanStatusException
   *$
   * @param status the status code that caused the exception
   */
  public UncleanStatusException(int status) {
    this(status, "Status code was non-zero");
  }

  /**
   * Create a new UncleanStatusException
   *$
   * @param message a message describing the exception
   */
  public UncleanStatusException(String message) {
    this(-1, message);
  }

  /**
   * Create a new UncleanStatusException
   */
  public UncleanStatusException() {
    this(-1, "Status code was non-zero");
  }

  /**
   * Create a new UncleanStatusException
   *$
   * @return the status code that caused the exception
   */
  public int getStatus() {
    return statusCode;
  }
}
