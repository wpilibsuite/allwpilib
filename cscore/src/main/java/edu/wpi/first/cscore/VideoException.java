// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** An exception raised by the camera server. */
public class VideoException extends RuntimeException {
  private static final long serialVersionUID = -9155939328084105145L;

  /**
   * Constructs the exception with the given message.
   *
   * @param msg The exception message.
   */
  public VideoException(String msg) {
    super(msg);
  }
}
