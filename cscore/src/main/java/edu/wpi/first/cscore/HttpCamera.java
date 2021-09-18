// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** A source that represents a MJPEG-over-HTTP (IP) camera. */
public class HttpCamera extends VideoCamera {
  public enum HttpCameraKind {
    kUnknown(0),
    kMJPGStreamer(1),
    kCSCore(2),
    kAxis(3);

    private final int value;

    HttpCameraKind(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }
  }

  /**
   * Convert from the numerical representation of kind to an enum type.
   *
   * @param kind The numerical representation of kind
   * @return The kind
   */
  public static HttpCameraKind getHttpCameraKindFromInt(int kind) {
    switch (kind) {
      case 1:
        return HttpCameraKind.kMJPGStreamer;
      case 2:
        return HttpCameraKind.kCSCore;
      case 3:
        return HttpCameraKind.kAxis;
      default:
        return HttpCameraKind.kUnknown;
    }
  }

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
   */
  public HttpCamera(String name, String url) {
    super(CameraServerJNI.createHttpCamera(name, url, HttpCameraKind.kUnknown.getValue()));
  }

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
   * @param kind Camera kind (e.g. kAxis)
   */
  public HttpCamera(String name, String url, HttpCameraKind kind) {
    super(CameraServerJNI.createHttpCamera(name, url, kind.getValue()));
  }

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param urls Array of Camera URLs
   */
  public HttpCamera(String name, String[] urls) {
    super(CameraServerJNI.createHttpCameraMulti(name, urls, HttpCameraKind.kUnknown.getValue()));
  }

  /**
   * Create a source for a MJPEG-over-HTTP (IP) camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param urls Array of Camera URLs
   * @param kind Camera kind (e.g. kAxis)
   */
  public HttpCamera(String name, String[] urls, HttpCameraKind kind) {
    super(CameraServerJNI.createHttpCameraMulti(name, urls, kind.getValue()));
  }

  /**
   * Get the kind of HTTP camera.
   *
   * <p>Autodetection can result in returning a different value than the camera was created with.
   *
   * @return The kind of HTTP camera.
   */
  public HttpCameraKind getHttpCameraKind() {
    return getHttpCameraKindFromInt(CameraServerJNI.getHttpCameraKind(m_handle));
  }

  /**
   * Change the URLs used to connect to the camera.
   *
   * @param urls Array of Camera URLs
   */
  public void setUrls(String[] urls) {
    CameraServerJNI.setHttpCameraUrls(m_handle, urls);
  }

  /**
   * Get the URLs used to connect to the camera.
   *
   * @return Array of camera URLs.
   */
  public String[] getUrls() {
    return CameraServerJNI.getHttpCameraUrls(m_handle);
  }
}
