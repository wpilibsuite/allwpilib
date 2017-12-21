/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
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
   * @param name Sink name (arbitrary unique identifier)
   * @param listenAddress TCP listen address (empty string for all addresses)
   * @param port TCP port number
   */
  public MjpegServer(String name, String listenAddress, int port) {
    super(CameraServerJNI.createMjpegServer(name, listenAddress, port));
  }

  /**
   * Create a MJPEG-over-HTTP server sink.
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
}
