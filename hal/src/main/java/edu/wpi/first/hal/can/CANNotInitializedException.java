// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
