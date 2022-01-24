// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

/** Class to resolve a service over mDNS. */
public class MulticastServiceResolver implements AutoCloseable {
  private final int m_handle;

  /**
   * Creates a MulticastServiceResolver.
   *
   * @param serviceType service type to look for
   */
  public MulticastServiceResolver(String serviceType) {
    m_handle = WPIUtilJNI.createMulticastServiceResolver(serviceType);
  }

  @Override
  public void close() {
    WPIUtilJNI.freeMulticastServiceResolver(m_handle);
  }

  public void start() {
    WPIUtilJNI.startMulticastServiceResolver(m_handle);
  }

  public void stop() {
    WPIUtilJNI.stopMulticastServiceResolver(m_handle);
  }

  public boolean hasImplementation() {
    return WPIUtilJNI.getMulticastServiceResolverHasImplementation(m_handle);
  }

  public int getEventHandle() {
    return WPIUtilJNI.getMulticastServiceResolverEventHandle(m_handle);
  }

  public ServiceData[] getData() {
    return WPIUtilJNI.getMulticastServiceResolverData(m_handle);
  }
}
