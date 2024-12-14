// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.net;

/** A web server using the HTTP protocol. */
public final class WebServer {
  private WebServer() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Create a web server at the given port. Note that local ports less than 1024 won't work as a
   * normal user. Also, many ports are blocked by the FRC robot radio; check the game manual for
   * what is allowed through the radio firewall.
   *
   * @param port local port number
   * @param path local path to document root
   */
  public static void start(int port, String path) {
    WPINetJNI.startWebServer(port, path);
  }

  /**
   * Stop web server running at the given port.
   *
   * @param port local port number
   */
  public static void stop(int port) {
    WPINetJNI.stopWebServer(port);
  }
}
