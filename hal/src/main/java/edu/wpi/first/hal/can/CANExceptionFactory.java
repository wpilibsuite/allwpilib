/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.can;

import edu.wpi.first.hal.communication.NIRioStatus;
import edu.wpi.first.hal.util.UncleanStatusException;

public final class CANExceptionFactory {
  // FRC Error codes
  static final int ERR_CANSessionMux_InvalidBuffer = -44086;
  static final int ERR_CANSessionMux_MessageNotFound = -44087;
  static final int ERR_CANSessionMux_NotAllowed = -44088;
  static final int ERR_CANSessionMux_NotInitialized = -44089;

  @SuppressWarnings({"JavadocMethod", "PMD.CyclomaticComplexity"})
  public static void checkStatus(int status, int messageID) throws CANInvalidBufferException,
      CANMessageNotAllowedException, CANNotInitializedException, UncleanStatusException {
    switch (status) {
      case NIRioStatus.kRioStatusSuccess:
        // Everything is ok... don't throw.
        return;
      case ERR_CANSessionMux_InvalidBuffer:
      case NIRioStatus.kRIOStatusBufferInvalidSize:
        throw new CANInvalidBufferException();
      case ERR_CANSessionMux_MessageNotFound:
      case NIRioStatus.kRIOStatusOperationTimedOut:
        throw new CANMessageNotFoundException();
      case ERR_CANSessionMux_NotAllowed:
      case NIRioStatus.kRIOStatusFeatureNotSupported:
        throw new CANMessageNotAllowedException("MessageID = " + Integer.toString(messageID));
      case ERR_CANSessionMux_NotInitialized:
      case NIRioStatus.kRIOStatusResourceNotInitialized:
        throw new CANNotInitializedException();
      default:
        throw new UncleanStatusException("Fatal status code detected:  " + Integer.toString(
            status));
    }
  }

  private CANExceptionFactory() {

  }
}
