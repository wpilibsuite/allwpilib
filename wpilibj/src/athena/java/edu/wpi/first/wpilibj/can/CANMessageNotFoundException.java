/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

/**
 * Exception indicating that a can message is not available from Network
 * Communications. This usually just means we already have the most recent value
 * cached locally.
 */
public class CANMessageNotFoundException extends RuntimeException {
  public CANMessageNotFoundException() {
    super();
  }
}
