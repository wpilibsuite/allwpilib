// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/**
 * A source that represents an Axis IP camera.
 *
 * @deprecated Use HttpCamera instead.
 */
@Deprecated(forRemoval = true, since = "2025")
public class AxisCamera extends HttpCamera {
  private static String hostToUrl(String host) {
    return "http://" + host + "/mjpg/video.mjpg";
  }

  private static String[] hostToUrl(String[] hosts) {
    String[] urls = new String[hosts.length];
    for (int i = 0; i < hosts.length; i++) {
      urls[i] = hostToUrl(hosts[i]);
    }
    return urls;
  }

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param host Camera host IP or DNS name (e.g. "10.x.y.11")
   */
  public AxisCamera(String name, String host) {
    super(name, hostToUrl(host), HttpCameraKind.kAxis);
  }

  /**
   * Create a source for an Axis IP camera.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param hosts Array of Camera host IPs/DNS names
   */
  public AxisCamera(String name, String[] hosts) {
    super(name, hostToUrl(hosts), HttpCameraKind.kAxis);
  }
}
