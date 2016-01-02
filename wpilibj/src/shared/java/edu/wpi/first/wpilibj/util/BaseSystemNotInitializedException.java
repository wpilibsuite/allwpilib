/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;


/**
 * Thrown if there is an error caused by a basic system or setting not being
 * properly initialized before being used.
 *$
 * @author Jonathan Leitschuh
 */
public class BaseSystemNotInitializedException extends RuntimeException {
  /**
   * Create a new BaseSystemNotInitializedException
   *$
   * @param message the message to attach to the exception
   */
  public BaseSystemNotInitializedException(String message) {
    super(message);
  }

  /**
   * Create a new BaseSystemNotInitializedException using the offending class
   * that was not set and the class that was affected.
   *$
   * @param offender The class or interface that was not properly initialized.
   * @param affected The class that was was affected by this missing
   *        initialization.
   */
  public BaseSystemNotInitializedException(Class<?> offender, Class<?> affected) {
    super("The " + offender.getSimpleName() + " for the " + affected.getSimpleName()
        + " was never set.");
  }

}
