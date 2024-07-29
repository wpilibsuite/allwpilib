// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.util;

/** Exception indicating that an error has occurred with a HAL Handle. */
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
