// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * Value change listener. This queues value change events matching the specified mask. Code using
 * the listener must periodically call readQueue() to read the events.
 */
public final class ValueListenerPoller implements AutoCloseable {
  /**
   * Construct a value listener poller.
   *
   * @param inst Instance
   */
  public ValueListenerPoller(NetworkTableInstance inst) {
    m_inst = inst;
    m_handle = NetworkTablesJNI.createValueListenerPoller(inst.getHandle());
  }

  /**
   * Start listening to value changes on a subscriber.
   *
   * @param subscriber Subscriber
   * @param eventMask Bitmask of ValueListenerFlags values
   * @return Listener handle
   */
  public int add(Subscriber subscriber, int eventMask) {
    return NetworkTablesJNI.addPolledValueListener(m_handle, subscriber.getHandle(), eventMask);
  }

  /**
   * Start listening to value changes on a subscriber.
   *
   * @param subscriber Subscriber
   * @param eventMask Bitmask of ValueListenerFlags values
   * @return Listener handle
   */
  public int add(MultiSubscriber subscriber, int eventMask) {
    return NetworkTablesJNI.addPolledValueListener(m_handle, subscriber.getHandle(), eventMask);
  }

  /**
   * Start listening to value changes on an entry.
   *
   * @param entry Entry
   * @param eventMask Bitmask of ValueListenerFlags values
   * @return Listener handle
   */
  public int add(NetworkTableEntry entry, int eventMask) {
    return NetworkTablesJNI.addPolledValueListener(m_handle, entry.getHandle(), eventMask);
  }

  /**
   * Remove a listener.
   *
   * @param listener Listener handle
   */
  public void remove(int listener) {
    NetworkTablesJNI.removeValueListener(listener);
  }

  /**
   * Read value notifications.
   *
   * @return Value notifications since the previous call to readQueue()
   */
  public ValueNotification[] readQueue() {
    return NetworkTablesJNI.readValueListenerQueue(m_inst, m_handle);
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      NetworkTablesJNI.destroyValueListenerPoller(m_handle);
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
