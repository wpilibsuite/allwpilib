// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.net;

import java.util.HashMap;
import java.util.Map;

/** Service data for MulticastServiceResolver. */
public class ServiceData {
  /**
   * Constructs a ServiceData.
   *
   * @param ipv4Address ipv4Address
   * @param port port
   * @param serviceName found service name
   * @param hostName found host name
   * @param keys txt keys
   * @param values txt values
   */
  public ServiceData(
      long ipv4Address,
      int port,
      String serviceName,
      String hostName,
      String[] keys,
      String[] values) {
    this.m_serviceName = serviceName;
    this.m_hostName = hostName;
    this.m_port = port;
    this.m_ipv4Address = ipv4Address;

    m_txt = new HashMap<>();

    for (int i = 0; i < keys.length; i++) {
      m_txt.put(keys[i], values[i]);
    }
  }

  /**
   * Returns service data payload.
   *
   * @return Service data payload.
   */
  public Map<String, String> getTxt() {
    return m_txt;
  }

  /**
   * Returns host name.
   *
   * @return Host name.
   */
  public String getHostName() {
    return m_hostName;
  }

  /**
   * Returns service name.
   *
   * @return Service name.
   */
  public String getServiceName() {
    return m_serviceName;
  }

  /**
   * Returns port number.
   *
   * @return Port number.
   */
  public int getPort() {
    return m_port;
  }

  /**
   * Returns IPv4 address.
   *
   * @return IPv4 address.
   */
  public long getIpv4Address() {
    return m_ipv4Address;
  }

  private final Map<String, String> m_txt;
  private final long m_ipv4Address;
  private final int m_port;
  private final String m_serviceName;
  private final String m_hostName;
}
