/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/// A source that represents a MJPEG-over-HTTP (IP) camera.
public class HttpCamera extends VideoSource {
  public enum CameraKind {
    kUnknown(0), kMJPGStreamer(1), kCSCore(2), kAxis(3);
    private int value;

    private CameraKind(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }
  }

  public static CameraKind getCameraKindFromInt(int kind) {
    switch (kind) {
      case 1: return CameraKind.kMJPGStreamer;
      case 2: return CameraKind.kCSCore;
      case 3: return CameraKind.kAxis;
      default: return CameraKind.kUnknown;
    }
  }

  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
  public HttpCamera(String name, String url) {
    super(CameraServerJNI.createHttpCamera(name, url, CameraKind.kUnknown.getValue()));
  }

  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
  /// @param kind Camera kind (e.g. kAxis)
  public HttpCamera(String name, String url, CameraKind kind) {
    super(CameraServerJNI.createHttpCamera(name, url, kind.getValue()));
  }

  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param urls Array of Camera URLs
  public HttpCamera(String name, String[] urls) {
    super(CameraServerJNI.createHttpCameraMulti(name, urls, CameraKind.kUnknown.getValue()));
  }

  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param urls Array of Camera URLs
  /// @param kind Camera kind (e.g. kAxis)
  public HttpCamera(String name, String[] urls, CameraKind kind) {
    super(CameraServerJNI.createHttpCameraMulti(name, urls, kind.getValue()));
  }

  /// Get the kind of HTTP camera.
  /// Autodetection can result in returning a different value than the camera
  /// was created with.
  public CameraKind getCameraKind() {
    return getCameraKindFromInt(CameraServerJNI.getHttpCameraKind(m_handle));
  }

  /// Change the URLs used to connect to the camera.
  public void setUrls(String[] urls) {
    CameraServerJNI.setHttpCameraUrls(m_handle, urls);
  }

  /// Get the URLs used to connect to the camera.
  public String[] getUrls() {
    return CameraServerJNI.getHttpCameraUrls(m_handle);
  }
}
