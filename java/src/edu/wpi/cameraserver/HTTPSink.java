package edu.wpi.cameraserver;

/// A sink that acts as a MJPEG-over-HTTP network server.
public class HTTPSink extends VideoSink {
  /// Create a MJPEG-over-HTTP server sink.
  /// @param name Sink name (arbitrary unique identifier)
  /// @param listenAddress TCP listen address (empty string for all addresses)
  /// @param port TCP port number
  public HTTPSink(String name, String listenAddress, int port) {
    super(CameraServerJNI.createHTTPSink(name, listenAddress, port));
  }

  /// Create a MJPEG-over-HTTP server sink.
  /// @param name Sink name (arbitrary unique identifier)
  /// @param port TCP port number
  public HTTPSink(String name, int port) {
    this(name, "", port);
  }

  /// Set what video channel should be served.
  /// MJPEG-HTTP can only serve a single channel of video.
  /// By default, channel 0 is served.
  /// @param channel video channel to serve to clients
  public void setSourceChannel(int channel) {
    CameraServerJNI.setSinkSourceChannel(m_handle, channel);
  }
}
