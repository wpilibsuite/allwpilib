// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.util;

/** Exception indicating that the resource is already allocated. */
public class AllocationException extends RuntimeException {
  /**
   * Create a new AllocationException.
   *
   * @param msg the message to attach to the exception
   */
  public AllocationException(String msg) {
    super(msg);
  }
}
