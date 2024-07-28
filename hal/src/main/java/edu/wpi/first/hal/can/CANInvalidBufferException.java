// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

/**
 * Exception indicating that a CAN driver library entry-point was passed an invalid buffer.
 * Typically, this is due to a buffer being too small to include the needed safety token.
 */
public class CANInvalidBufferException extends RuntimeException {
  private static final long serialVersionUID = -7993785672956997939L;

  /** Constructs a new CANInvalidBufferException with no message. */
  public CANInvalidBufferException() {
    super();
  }

  /**
   * Constructs a new CANInvalidBufferException with {@code msg} as its detail message.
   *
   * @param msg the message
   */
  public CANInvalidBufferException(String msg) {
    super(msg);
  }
}
