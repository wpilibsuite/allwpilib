// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.net;

/**
 * Forward ports to another host. This is primarily useful for accessing Ethernet-connected devices
 * from a computer tethered to the RoboRIO USB port.
 */
public final class PortForwarder {
  private PortForwarder() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Forward a local TCP port to a remote host and port. Note that local ports less than 1024 won't
   * work as a normal user.
   *
   * @param port local port number
   * @param remoteHost remote IP address / DNS name
   * @param remotePort remote port number
   */
  public static void add(int port, String remoteHost, int remotePort) {
    WPINetJNI.addPortForwarder(port, remoteHost, remotePort);
  }

  /**
   * Stop TCP forwarding on a port.
   *
   * @param port local port number
   */
  public static void remove(int port) {
    WPINetJNI.removePortForwarder(port);
  }
}
