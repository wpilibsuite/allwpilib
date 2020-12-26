// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

/**
 * This exception will be thrown if a command is used illegally. There are several ways for this to
 * happen.
 *
 * <p> Basically, a command becomes "locked" after it is first started or added to a command group.
 * </p>
 *
 * <p> This exception should be thrown if (after a command has been locked) its requirements change,
 * it is put into multiple command groups, it is started from outside its command group, or it adds
 * a new child. </p>
 */
@SuppressWarnings("serial")
public class IllegalUseOfCommandException extends RuntimeException {
  /**
   * Instantiates an {@link IllegalUseOfCommandException}.
   */
  public IllegalUseOfCommandException() {
  }

  /**
   * Instantiates an {@link IllegalUseOfCommandException} with the given message.
   *
   * @param message the message
   */
  public IllegalUseOfCommandException(String message) {
    super(message);
  }
}
