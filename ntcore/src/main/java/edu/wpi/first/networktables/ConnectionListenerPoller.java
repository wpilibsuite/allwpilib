// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * A connection listener. This queues connection notifications. Code using the listener must
 * periodically call readQueue() to read the notifications.
 */
public final class ConnectionListenerPoller implements AutoCloseable {
  /**
   * Construct a connection listener poller.
   *
   * @param inst Instance
   */
  public ConnectionListenerPoller(NetworkTableInstance inst) {
    m_inst = inst;
    m_handle = NetworkTablesJNI.createConnectionListenerPoller(inst.getHandle());
  }

  /**
   * Create a connection listener.
   *
   * @param immediateNotify if notification should be immediately created for existing connections
   * @return Listener handle
   */
  public int add(boolean immediateNotify) {
    return NetworkTablesJNI.addPolledConnectionListener(m_handle, immediateNotify);
  }

  /**
   * Remove a connection listener.
   *
   * @param listener Listener handle
   */
  public void remove(int listener) {
    NetworkTablesJNI.removeConnectionListener(listener);
  }

  /**
   * Read connection notifications.
   *
   * @return Connection notifications since the previous call to readQueue()
   */
  public ConnectionNotification[] readQueue() {
    return NetworkTablesJNI.readConnectionListenerQueue(m_inst, m_handle);
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      NetworkTablesJNI.destroyConnectionListenerPoller(m_handle);
    }
    m_handle = 0;
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
