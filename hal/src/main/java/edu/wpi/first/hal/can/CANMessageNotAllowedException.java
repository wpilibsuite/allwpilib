// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

/**
 * Exception indicating that the Jaguar CAN Driver layer refused to send a restricted message ID to
 * the CAN bus.
 */
public class CANMessageNotAllowedException extends RuntimeException {
  private static final long serialVersionUID = -638450112427013494L;

  /**
   * Constructs a new CANMessageNotAllowedException with {@code msg} as its detail message.
   *
   * @param msg the message
   */
  public CANMessageNotAllowedException(String msg) {
    super(msg);
  }
}
