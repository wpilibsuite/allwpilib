// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import java.util.function.Consumer;

/**
 * Topic change listener. This calls back to a callback function when a topic change matching the
 * specified mask occurs. The callback function is called asynchronously on a separate thread, so
 * it's important to use synchronization or atomics when accessing any shared state from the
 * callback function.
 */
public final class TopicListener implements AutoCloseable {
  /**
   * Create a listener for changes on a particular topic.
   *
   * @param topic Topic
   * @param eventMask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  public TopicListener(Topic topic, int eventMask, Consumer<TopicNotification> listener) {
    m_inst = topic.getInstance();
    m_handle = m_inst.addTopicListener(topic, eventMask, listener);
  }

  /**
   * Create a listener for topic changes on a subscriber. This does NOT keep the subscriber active.
   *
   * @param subscriber Subscriber
   * @param eventMask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  public TopicListener(Subscriber subscriber, int eventMask, Consumer<TopicNotification> listener) {
    m_inst = subscriber.getTopic().getInstance();
    m_handle = m_inst.addTopicListener(subscriber, eventMask, listener);
  }

  /**
   * Create a listener for topic changes on a subscriber. This does NOT keep the subscriber active.
   *
   * @param subscriber Subscriber
   * @param eventMask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  public TopicListener(
      MultiSubscriber subscriber, int eventMask, Consumer<TopicNotification> listener) {
    m_inst = subscriber.getInstance();
    m_handle = m_inst.addTopicListener(subscriber, eventMask, listener);
  }

  /**
   * Create a listener for topic changes on an entry.
   *
   * @param entry Entry
   * @param eventMask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  public TopicListener(
      NetworkTableEntry entry, int eventMask, Consumer<TopicNotification> listener) {
    m_inst = entry.getInstance();
    m_handle = m_inst.addTopicListener(entry, eventMask, listener);
  }

  /**
   * Create a listener for changes to topics with names that start with any of the given prefixes.
   *
   * @param inst Instance
   * @param prefixes Topic name string prefixes
   * @param eventMask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  public TopicListener(
      NetworkTableInstance inst,
      String[] prefixes,
      int eventMask,
      Consumer<TopicNotification> listener) {
    m_inst = inst;
    m_handle = inst.addTopicListener(prefixes, eventMask, listener);
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      m_inst.removeTopicListener(m_handle);
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
   * Wait for the topic listener queue to be empty. This is primarily useful for deterministic
   * testing. This blocks until either the topic listener queue is empty (e.g. there are no more
   * events that need to be passed along to callbacks or poll queues) or the timeout expires.
   *
   * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or a negative value to
   *     block indefinitely
   * @return False if timed out, otherwise true.
   */
  public boolean waitForQueue(double timeout) {
    return m_inst.waitForTopicListenerQueue(timeout);
  }

  private final NetworkTableInstance m_inst;
  private int m_handle;
}
