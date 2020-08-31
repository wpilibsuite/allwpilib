/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.util;

/**
 * Exception indicating that an error has occurred with a HAL Handle.
 */
@SuppressWarnings("serial")
public class HalHandleException extends RuntimeException {
  /**
   * Create a new HalHandleException.
   *
   * @param msg the message to attach to the exception
   */
  public HalHandleException(String msg) {
    super(msg);
  }
}
