/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import edu.wpi.first.wpilibj.communication.NIRioStatus;
import edu.wpi.first.wpilibj.util.UncleanStatusException;

/**
 * Exception indicating that the Jaguar CAN Driver layer refused to send a
 * restricted message ID to the CAN bus.
 */
public class CANExceptionFactory {
    // FRC Error codes
    static final int ERR_JaguarCANDriver_InvalidBuffer	=	-44086;
    static final int ERR_JaguarCANDriver_TimedOut	=	-44087;
    static final int ERR_JaguarCANDriver_NotAllowed	=	-44088;
    static final int ERR_JaguarCANDriver_NotInitialized	=	-44089;

    public static void checkStatus(int status, int messageID) throws
        CANInvalidBufferException, CANTimeoutException,
        CANMessageNotAllowedException, CANNotInitializedException,
        UncleanStatusException {
        switch (status) {
        case NIRioStatus.kRioStatusSuccess:
            // Everything is ok... don't throw.
            return;
        case ERR_JaguarCANDriver_InvalidBuffer:
        case NIRioStatus.kRIOStatusBufferInvalidSize:
            throw new CANInvalidBufferException();
        case ERR_JaguarCANDriver_TimedOut:
        case NIRioStatus.kRIOStatusOperationTimedOut:
            throw new CANTimeoutException();
        case ERR_JaguarCANDriver_NotAllowed:
        case NIRioStatus.kRIOStatusFeatureNotSupported:
            throw new CANMessageNotAllowedException("MessageID = " + Integer.toString(messageID));
        case ERR_JaguarCANDriver_NotInitialized:
        case NIRioStatus.kRIOStatusResourceNotInitialized:
            throw new CANNotInitializedException();
        default:
            throw new UncleanStatusException("Fatal status code detected:  " + Integer.toString(status));
        }
    }
}
