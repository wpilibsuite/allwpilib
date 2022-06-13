// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.util;

/** Exception for bad status codes from the chip object. */
public final class UncleanStatusException extends IllegalStateException {
  private final int m_statusCode;

  /**
   * Create a new UncleanStatusException.
   *
   * @param status the status code that caused the exception
   * @param message A message describing the exception
   */
  public UncleanStatusException(int status, String message) {
    super(message);
    m_statusCode = status;
  }

  /**
   * Create a new UncleanStatusException.
   *
   * @param status the status code that caused the exception
   */
  public UncleanStatusException(int status) {
    this(status, "Status code was non-zero");
  }

  /**
   * Create a new UncleanStatusException.
   *
   * @param message a message describing the exception
   */
  public UncleanStatusException(String message) {
    this(-1, message);
  }

  /** Create a new UncleanStatusException. */
  public UncleanStatusException() {
    this(-1, "Status code was non-zero");
  }

  /**
   * Create a new UncleanStatusException.
   *
   * @return the status code that caused the exception
   */
  public int getStatus() {
    return m_statusCode;
  }
}
