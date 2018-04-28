/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/**
 * An exception raised by the camera server.
 */
public class VideoException extends RuntimeException {
  private static final long serialVersionUID = -9155939328084105145L;

  public VideoException(String msg) {
    super(msg);
  }

  @Override
  public String toString() {
    return "VideoException [" + super.toString() + "]";
  }
}
