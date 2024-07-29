// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.can;

import edu.wpi.first.hal.communication.NIRioStatus;
import edu.wpi.first.hal.util.UncleanStatusException;

/**
 * Checks the status of a CAN message and throws an exception of the appropriate type if necessary.
 */
public final class CANExceptionFactory {
  // FRC Error codes
  static final int ERR_CANSessionMux_InvalidBuffer = -44086;
  static final int ERR_CANSessionMux_MessageNotFound = -44087;
  static final int ERR_CANSessionMux_NotAllowed = -44088;
  static final int ERR_CANSessionMux_NotInitialized = -44089;

  /**
   * Checks the status of a CAN message with the given message ID.
   *
   * @param status The CAN status.
   * @param messageID The CAN message ID.
   * @throws CANInvalidBufferException if the buffer is invalid.
   * @throws CANMessageNotAllowedException if the message isn't allowed.
   * @throws CANNotInitializedException if the CAN bus isn't initialized.
   * @throws UncleanStatusException if the status code passed in reports an error.
   */
  public static void checkStatus(int status, int messageID) {
    switch (status) {
      case NIRioStatus.kRioStatusSuccess -> {
        // Everything is ok... don't throw.
      }
      case ERR_CANSessionMux_InvalidBuffer,
          NIRioStatus.kRIOStatusBufferInvalidSize -> throw new CANInvalidBufferException();
      case ERR_CANSessionMux_MessageNotFound,
          NIRioStatus.kRIOStatusOperationTimedOut -> throw new CANMessageNotFoundException();
      case ERR_CANSessionMux_NotAllowed,
          NIRioStatus.kRIOStatusFeatureNotSupported -> throw new CANMessageNotAllowedException(
          "MessageID = " + messageID);
      case ERR_CANSessionMux_NotInitialized,
          NIRioStatus.kRIOStatusResourceNotInitialized -> throw new CANNotInitializedException();
      default -> throw new UncleanStatusException("Fatal status code detected:  " + status);
    }
  }

  private CANExceptionFactory() {}
}
