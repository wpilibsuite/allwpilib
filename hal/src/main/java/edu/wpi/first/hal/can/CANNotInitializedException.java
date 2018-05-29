/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.can;

/**
 * Exception indicating that the CAN driver layer has not been initialized. This happens when an
 * entry-point is called before a CAN driver plugin has been installed.
 */
public class CANNotInitializedException extends RuntimeException {
  private static final long serialVersionUID = -5982895147092686594L;

  public CANNotInitializedException() {
    super();
  }

  public CANNotInitializedException(String msg) {
    super(msg);
  }
}
