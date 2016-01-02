/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

/**
 * Exception indicating that a CAN driver library entry-point was passed an
 * invalid buffer. Typically, this is due to a buffer being too small to include
 * the needed safety token.
 */
public class CANInvalidBufferException extends RuntimeException {
  public CANInvalidBufferException() {
    super();
  }
}
