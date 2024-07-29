// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.net;

import edu.wpi.first.util.WPICleaner;
import java.lang.ref.Cleaner.Cleanable;
import java.util.Map;

/** Class to announce over mDNS that a service is available. */
public class MulticastServiceAnnouncer implements AutoCloseable {
  private final int m_handle;
  private final Cleanable m_cleanable;

  private static Runnable cleanupAction(int handle) {
    return () -> WPINetJNI.freeMulticastServiceAnnouncer(handle);
  }

  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   * @param txt txt
   */
  @SuppressWarnings("this-escape")
  public MulticastServiceAnnouncer(
      String serviceName, String serviceType, int port, Map<String, String> txt) {
    String[] keys = txt.keySet().toArray(String[]::new);
    String[] values = txt.values().toArray(String[]::new);
    m_handle =
        WPINetJNI.createMulticastServiceAnnouncer(serviceName, serviceType, port, keys, values);
    m_cleanable = WPICleaner.register(this, cleanupAction(m_handle));
  }

  /**
   * Creates a MulticastServiceAnnouncer.
   *
   * @param serviceName service name
   * @param serviceType service type
   * @param port port
   */
  @SuppressWarnings("this-escape")
  public MulticastServiceAnnouncer(String serviceName, String serviceType, int port) {
    m_handle =
        WPINetJNI.createMulticastServiceAnnouncer(serviceName, serviceType, port, null, null);
    m_cleanable = WPICleaner.register(this, cleanupAction(m_handle));
  }

  @Override
  public void close() {
    m_cleanable.clean();
  }

  /** Starts multicast service announcer. */
  public void start() {
    WPINetJNI.startMulticastServiceAnnouncer(m_handle);
  }

  /** Stops multicast service announcer. */
  public void stop() {
    WPINetJNI.stopMulticastServiceAnnouncer(m_handle);
  }

  /**
   * Returns true if there's a multicast service announcer implementation.
   *
   * @return True if there's a multicast service announcer implementation.
   */
  public boolean hasImplementation() {
    return WPINetJNI.getMulticastServiceAnnouncerHasImplementation(m_handle);
  }
}
