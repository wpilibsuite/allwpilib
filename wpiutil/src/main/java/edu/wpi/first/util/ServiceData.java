// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.util.HashMap;
import java.util.Map;

public class ServiceData {
  public ServiceData(long ipv4Address, int port, String serviceName, String hostName, String[] keys, String[] values) {
    this.serviceName = serviceName;
    this.hostName = hostName;
    this.port = port;
    this.ipv4Address = ipv4Address;

    txt = new HashMap<>();

    for(int i = 0; i < keys.length; i++) {
      txt.put(keys[i], values[i]);
    }
  }

  public Map<String, String> getTxt() {
    return txt;
  }

  public String getHostName() {
    return hostName;
  }

  public String getServiceName() {
    return serviceName;
  }

  public int getPort() {
    return port;
  }

  public long getIpv4Address() {
    return ipv4Address;
  }

  private final Map<String, String> txt;
  private final long ipv4Address;
  private final int port;
  private final String serviceName;
  private final String hostName;
}
