// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import java.util.function.Consumer;

/**
 * Connection listener. This calls back to a callback function when a connection change occurs. The
 * callback function is called asynchronously on a separate thread, so it's important to use
 * synchronization or atomics when accessing any shared state from the callback function.
 */
public final class ConnectionListener implements AutoCloseable {
  /**
   * Create a listener for connection changes.
   *
   * @param inst Instance
   * @param immediateNotify if notification should be immediately created for existing connections
   * @param listener Listener function
   */
  public ConnectionListener(
      NetworkTableInstance inst,
      boolean immediateNotify,
      Consumer<ConnectionNotification> listener) {
    m_inst = inst;
    m_handle = inst.addConnectionListener(immediateNotify, listener);
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      m_inst.removeConnectionListener(m_handle);
      m_handle = 0;
    }
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
   * @return Native handle
   */
  public int getHandle() {
    return m_handle;
  }

  /**
   * Wait for the connection listener queue to be empty. This is primarily useful for deterministic
   * testing. This blocks until either the connection listener queue is empty (e.g. there are no
   * more events that need to be passed along to callbacks or poll queues) or the timeout expires.
   *
   * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or a negative value to
   *     block indefinitely
   * @return False if timed out, otherwise true.
   */
  public boolean waitForQueue(double timeout) {
    return m_inst.waitForConnectionListenerQueue(timeout);
  }

  private final NetworkTableInstance m_inst;
  private int m_handle;
}
