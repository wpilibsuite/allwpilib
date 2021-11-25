// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.util.Map;

public class MulticastServiceAnnouncer implements AutoCloseable {
  private final int handle;

  public MulticastServiceAnnouncer(String serviceName, String serviceType, int port, Map<String, String> txt) {
    String[] keys = txt.keySet().toArray(String[]::new);
    String[] values = txt.values().toArray(String[]::new);
    handle = WPIUtilJNI.createMulticastServiceAnnouncer(serviceName, serviceType, port, keys, values);
  }

  @Override
  public void close() {
    WPIUtilJNI.freeMulticastServiceAnnouncer(handle);
  }

  public void start() {
    WPIUtilJNI.startMulticastServiceAnnouncer(handle);
  }

  public void stop() {
    WPIUtilJNI.stopMulticastServiceAnnouncer(handle);
  }

  public boolean hasImplementation() {
    return WPIUtilJNI.getMulticastServiceAnnouncerHasImplementation(handle);
  }
}
