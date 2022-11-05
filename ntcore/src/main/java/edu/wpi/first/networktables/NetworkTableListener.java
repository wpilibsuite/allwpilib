// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import java.util.EnumSet;
import java.util.function.Consumer;

/**
 * Event listener. This calls back to a callback function when an event matching the specified mask
 * occurs. The callback function is called asynchronously on a separate thread, so it's important to
 * use synchronization or atomics when accessing any shared state from the callback function.
 */
public final class NetworkTableListener implements AutoCloseable {
  /**
   * Create a listener for changes to topics with names that start with any of the given prefixes.
   * This creates a corresponding internal subscriber with the lifetime of the listener.
   *
   * @param inst Instance
   * @param prefixes Topic name string prefixes
   * @param eventKinds set of event kinds to listen to
   * @param listener Listener function
   * @return Listener
   */
  public static NetworkTableListener createListener(
      NetworkTableInstance inst,
      String[] prefixes,
      EnumSet<NetworkTableEvent.Kind> eventKinds,
      Consumer<NetworkTableEvent> listener) {
    return new NetworkTableListener(inst, inst.addListener(prefixes, eventKinds, listener));
  }

  /**
   * Create a listener for changes on a particular topic. This creates a corresponding internal
   * subscriber with the lifetime of the listener.
   *
   * @param topic Topic
   * @param eventKinds set of event kinds to listen to
   * @param listener Listener function
   * @return Listener
   */
  public static NetworkTableListener createListener(
      Topic topic,
      EnumSet<NetworkTableEvent.Kind> eventKinds,
      Consumer<NetworkTableEvent> listener) {
    NetworkTableInstance inst = topic.getInstance();
    return new NetworkTableListener(inst, inst.addListener(topic, eventKinds, listener));
  }

  /**
   * Create a listener for topic changes on a subscriber. This does NOT keep the subscriber active.
   *
   * @param subscriber Subscriber
   * @param eventKinds set of event kinds to listen to
   * @param listener Listener function
   * @return Listener
   */
  public static NetworkTableListener createListener(
      Subscriber subscriber,
      EnumSet<NetworkTableEvent.Kind> eventKinds,
      Consumer<NetworkTableEvent> listener) {
    NetworkTableInstance inst = subscriber.getTopic().getInstance();
    return new NetworkTableListener(inst, inst.addListener(subscriber, eventKinds, listener));
  }

  /**
   * Create a listener for topic changes on a subscriber. This does NOT keep the subscriber active.
   *
   * @param subscriber Subscriber
   * @param eventKinds set of event kinds to listen to
   * @param listener Listener function
   * @return Listener
   */
  public static NetworkTableListener createListener(
      MultiSubscriber subscriber,
      EnumSet<NetworkTableEvent.Kind> eventKinds,
      Consumer<NetworkTableEvent> listener) {
    NetworkTableInstance inst = subscriber.getInstance();
    return new NetworkTableListener(inst, inst.addListener(subscriber, eventKinds, listener));
  }

  /**
   * Create a listener for topic changes on an entry.
   *
   * @param entry Entry
   * @param eventKinds set of event kinds to listen to
   * @param listener Listener function
   * @return Listener
   */
  public static NetworkTableListener createListener(
      NetworkTableEntry entry,
      EnumSet<NetworkTableEvent.Kind> eventKinds,
      Consumer<NetworkTableEvent> listener) {
    NetworkTableInstance inst = entry.getInstance();
    return new NetworkTableListener(inst, inst.addListener(entry, eventKinds, listener));
  }

  /**
   * Create a connection listener.
   *
   * @param inst instance
   * @param immediateNotify notify listener of all existing connections
   * @param listener listener function
   * @return Listener
   */
  public static NetworkTableListener createConnectionListener(
      NetworkTableInstance inst, boolean immediateNotify, Consumer<NetworkTableEvent> listener) {
    return new NetworkTableListener(inst, inst.addConnectionListener(immediateNotify, listener));
  }

  /**
   * Create a listener for log messages. By default, log messages are sent to stderr; this function
   * sends log messages with the specified levels to the provided callback function instead. The
   * callback function will only be called for log messages with level greater than or equal to
   * minLevel and less than or equal to maxLevel; messages outside this range will be silently
   * ignored.
   *
   * @param inst instance
   * @param minLevel minimum log level
   * @param maxLevel maximum log level
   * @param listener listener function
   * @return Listener
   */
  public static NetworkTableListener createLogger(
      NetworkTableInstance inst, int minLevel, int maxLevel, Consumer<NetworkTableEvent> listener) {
    return new NetworkTableListener(inst, inst.addLogger(minLevel, maxLevel, listener));
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      m_inst.removeListener(m_handle);
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
    return m_inst.waitForListenerQueue(timeout);
  }

  private NetworkTableListener(NetworkTableInstance inst, int handle) {
    m_inst = inst;
    m_handle = handle;
  }

  private final NetworkTableInstance m_inst;
  private int m_handle;
}
