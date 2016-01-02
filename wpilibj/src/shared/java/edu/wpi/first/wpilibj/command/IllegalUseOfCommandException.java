/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * This exception will be thrown if a command is used illegally. There are
 * several ways for this to happen.
 *
 * <p>
 * Basically, a command becomes "locked" after it is first started or added to a
 * command group.
 * </p>
 *
 * <p>
 * This exception should be thrown if (after a command has been locked) its
 * requirements change, it is put into multiple command groups, it is started
 * from outside its command group, or it adds a new child.
 * </p>
 *
 * @author Joe Grinstead
 */
public class IllegalUseOfCommandException extends RuntimeException {

  /**
   * Instantiates an {@link IllegalUseOfCommandException}.
   */
  public IllegalUseOfCommandException() {}

  /**
   * Instantiates an {@link IllegalUseOfCommandException} with the given
   * message.
   *$
   * @param message the message
   */
  public IllegalUseOfCommandException(String message) {
    super(message);
  }
}
