package edu.wpi.cameraserver;

/// A source that represents a MJPEG-over-HTTP (IP) camera.
public class HTTPCamera extends VideoSource {
  /// Create a source for a MJPEG-over-HTTP (IP) camera.
  /// @param name Source name (arbitrary unique identifier)
  /// @param url Camera URL (e.g. "http://10.x.y.11/video/stream.mjpg")
  public HTTPCamera(String name, String url) {
    super(CameraServerJNI.createHTTPSource(name, url));
  }
}
