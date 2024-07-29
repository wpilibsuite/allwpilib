// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

/**
 * Exception indicating that a can message is not available from Network Communications. This
 * usually just means we already have the most recent value cached locally.
 */
public class CANMessageNotFoundException extends RuntimeException {
  private static final long serialVersionUID = 8249780881928189975L;

  /** Constructs a new CANMessageNotFoundException with no message. */
  public CANMessageNotFoundException() {
    super();
  }

  /**
   * Constructs a new CANMessageNotFoundException with {@code msg} as its detail message.
   *
   * @param msg the message
   */
  public CANMessageNotFoundException(String msg) {
    super(msg);
  }
}
