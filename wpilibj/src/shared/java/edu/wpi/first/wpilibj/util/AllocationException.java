/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

/**
 * Exception indicating that the resource is already allocated
 *$
 * @author dtjones
 */
public class AllocationException extends RuntimeException {

  /**
   * Create a new AllocationException
   *$
   * @param msg the message to attach to the exception
   */
  public AllocationException(String msg) {
    super(msg);
  }
}
