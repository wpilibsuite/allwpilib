// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * Topic change listener. This queues topic change events matching the specified mask. Code using
 * the listener must periodically call readQueue() to read the events.
 */
public final class TopicListenerPoller implements AutoCloseable {
  /**
   * Construct a topic listener poller.
   *
   * @param inst Instance
   */
  public TopicListenerPoller(NetworkTableInstance inst) {
    m_inst = inst;
    m_handle = NetworkTablesJNI.createTopicListenerPoller(inst.getHandle());
  }

  /**
   * Start listening to changes to a particular topic.
   *
   * @param topic Topic
   * @param eventMask Bitmask of TopicListenerFlags values
   * @return Listener handle
   */
  public int add(Topic topic, int eventMask) {
    return NetworkTablesJNI.addPolledTopicListener(m_handle, topic.getHandle(), eventMask);
  }

  /**
   * Start listening to topic changes for topics with names that start with any of the given
   * prefixes.
   *
   * @param prefixes Topic name string prefixes
   * @param eventMask Bitmask of TopicListenerFlags values
   * @return Listener handle
   */
  public int add(String[] prefixes, int eventMask) {
    return NetworkTablesJNI.addPolledTopicListener(m_handle, prefixes, eventMask);
  }

  /**
   * Remove a listener.
   *
   * @param listener Listener handle
   */
  public void remove(int listener) {
    NetworkTablesJNI.removeTopicListener(listener);
  }

  /**
   * Read topic notifications.
   *
   * @return Topic notifications since the previous call to readQueue()
   */
  public TopicNotification[] readQueue() {
    return NetworkTablesJNI.readTopicListenerQueue(m_inst, m_handle);
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      NetworkTablesJNI.destroyTopicListenerPoller(m_handle);
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
