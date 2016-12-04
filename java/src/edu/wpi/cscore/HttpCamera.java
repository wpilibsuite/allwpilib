/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/// A source that represents a MJPEG-over-HTTP (IP) camera.
public class HttpCamera extends VideoSource {
  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
  public HttpCamera(String name, String url) {
    super(CameraServerJNI.createHttpCamera(name, url));
  }
}
