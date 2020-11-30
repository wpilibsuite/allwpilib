/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/**
 * A sink that acts as a MJPEG-over-HTTP network server.
 */
public class MjpegServer extends VideoSink {
  /**
   * Create a MJPEG-over-HTTP server sink.
   *
   * @param name Sink name (arbitrary unique identifier)
   * @param listenAddress TCP listen address (empty string for all addresses)
   * @param port TCP port number
   */
  public MjpegServer(String name, String listenAddress, int port) {
    super(CameraServerJNI.createMjpegServer(name, listenAddress, port));
  }

  /**
   * Create a MJPEG-over-HTTP server sink.
   *
   * @param name Sink name (arbitrary unique identifier)
   * @param port TCP port number
   */
  public MjpegServer(String name, int port) {
    this(name, "", port);
  }

  /**
   * Get the listen address of the server.
   */
  public String getListenAddress() {
    return CameraServerJNI.getMjpegServerListenAddress(m_handle);
  }

  /**
   * Get the port number of the server.
   */
  public int getPort() {
    return CameraServerJNI.getMjpegServerPort(m_handle);
  }

  /**
   * Set the stream resolution for clients that don't specify it.
   *
   * <p>It is not necessary to set this if it is the same as the source
   * resolution.
   *
   * <p>Setting this different than the source resolution will result in
   * increased CPU usage, particularly for MJPEG source cameras, as it will
   * decompress, resize, and recompress the image, instead of using the
   * camera's MJPEG image directly.
   *
   * @param width width, 0 for unspecified
   * @param height height, 0 for unspecified
   */
  public void setResolution(int width, int height) {
    CameraServerJNI.setProperty(CameraServerJNI.getSinkProperty(m_handle, "width"), width);
    CameraServerJNI.setProperty(CameraServerJNI.getSinkProperty(m_handle, "height"), height);
  }

  /**
   * Set the stream frames per second (FPS) for clients that don't specify it.
   *
   * <p>It is not necessary to set this if it is the same as the source FPS.
   *
   * @param fps FPS, 0 for unspecified
   */
  public void setFPS(int fps) {
    CameraServerJNI.setProperty(CameraServerJNI.getSinkProperty(m_handle, "fps"), fps);
  }

  /**
   * Set the compression for clients that don't specify it.
   *
   * <p>Setting this will result in increased CPU usage for MJPEG source cameras
   * as it will decompress and recompress the image instead of using the
   * camera's MJPEG image directly.
   *
   * @param quality JPEG compression quality (0-100), -1 for unspecified
   */
  public void setCompression(int quality) {
    CameraServerJNI.setProperty(CameraServerJNI.getSinkProperty(m_handle, "compression"),
                                quality);
  }

  /**
   * Set the default compression used for non-MJPEG sources.  If not set,
   * 80 is used.  This function has no effect on MJPEG source cameras; use
   * SetCompression() instead to force recompression of MJPEG source images.
   *
   * @param quality JPEG compression quality (0-100)
   */
  public void setDefaultCompression(int quality) {
    CameraServerJNI.setProperty(CameraServerJNI.getSinkProperty(m_handle, "default_compression"),
                                quality);
  }
}
