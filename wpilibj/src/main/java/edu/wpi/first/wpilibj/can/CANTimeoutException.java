/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import java.io.IOException;

/**
 * Exception indicating that the CAN device did not respond
 * within the timeout period specified.
 */
public class CANTimeoutException extends IOException {
    public CANTimeoutException() {
        super();
    }
}
