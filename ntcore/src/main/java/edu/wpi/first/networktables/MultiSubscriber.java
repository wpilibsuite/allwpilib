// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * Subscribe to multiple topics based on one or more topic name prefixes. Can be used in combination
 * with NetworkTableListenerPoller to listen for value changes across all matching topics.
 */
public final class MultiSubscriber implements AutoCloseable {
  /**
   * Create a multiple subscriber.
   *
   * @param inst instance
   * @param prefixes topic name prefixes
   * @param options subscriber options
   */
  public MultiSubscriber(NetworkTableInstance inst, String[] prefixes, PubSubOption... options) {
    m_inst = inst;
    m_handle = NetworkTablesJNI.subscribeMultiple(inst.getHandle(), prefixes, options);
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      NetworkTablesJNI.unsubscribeMultiple(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Gets the instance for the subscriber.
   *
   * @return Instance
   */
  public NetworkTableInstance getInstance() {
    return m_inst;
  }

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  public boolean isValid() {
    return m_handle != 0;
  }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  public int getHandle() {
    return m_handle;
  }

  private final NetworkTableInstance m_inst;
  private int m_handle;
}
