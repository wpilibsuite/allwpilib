/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

/**
 * Exception indicating that the resource is already allocated This is meant to
 * be thrown by the resource class
 *$
 * @author dtjones
 */
public class CheckedAllocationException extends Exception {

  /**
   * Create a new CheckedAllocationException
   *$
   * @param msg the message to attach to the exception
   */
  public CheckedAllocationException(String msg) {
    super(msg);
  }
}
