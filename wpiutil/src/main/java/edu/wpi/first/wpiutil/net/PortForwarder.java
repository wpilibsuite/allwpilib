/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.net;

import edu.wpi.first.wpiutil.WPIUtilJNI;

/**
 * Forward ports to another host. This is primarily useful for accessing
 * Ethernet-connected devices from a computer tethered to the RoboRIO USB port.
 */
public final class PortForwarder {
  private PortForwarder() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Forward a local TCP port to a remote host and port.
   * Note that local ports less than 1024 won't work as a normal user.
   *
   * @param port       local port number
   * @param remoteHost remote IP address / DNS name
   * @param remotePort remote port number
   */
  public static void add(int port, String remoteHost, int remotePort) {
    WPIUtilJNI.addPortForwarder(port, remoteHost, remotePort);
  }

  /**
   * Stop TCP forwarding on a port.
   *
   * @param port local port number
   */
  public static void remove(int port) {
    WPIUtilJNI.removePortForwarder(port);
  }
}
