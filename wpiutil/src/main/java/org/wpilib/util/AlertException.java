// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

/** Exception thrown to indicate an alert condition with an associated reason code. */
public class AlertException extends RuntimeException {
  private static final long serialVersionUID = -4273483424L;

  private final int m_reason;

  /**
   * Constructs a new AlertException with the specified detail message and reason code.
   *
   * @param message the detail message
   * @param reason the reason code associated with this exception
   */
  public AlertException(String message, int reason) {
    super(message);
    this.m_reason = reason;
  }

  /**
   * Gets the reason code associated with this exception.
   *
   * @return the reason code
   */
  public int getReason() {
    return m_reason;
  }
}
