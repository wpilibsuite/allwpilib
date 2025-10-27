// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.util;

/**
 * Exception indicating that the resource is already allocated. This is meant to be thrown by the
 * resource class.
 */
public class CheckedAllocationException extends Exception {
  /**
   * Create a new CheckedAllocationException.
   *
   * @param msg the message to attach to the exception
   */
  public CheckedAllocationException(String msg) {
    super(msg);
  }
}
