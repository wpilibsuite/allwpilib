/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

/**
 * Exception indicating that an error has occured with a HAL Handle.
 */
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
