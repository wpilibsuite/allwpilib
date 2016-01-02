/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

/**
 * Exception indicating that the Jaguar CAN Driver layer refused to send a
 * restricted message ID to the CAN bus.
 */
public class CANMessageNotAllowedException extends RuntimeException {
  public CANMessageNotAllowedException(String msg) {
    super(msg);
  }
}
