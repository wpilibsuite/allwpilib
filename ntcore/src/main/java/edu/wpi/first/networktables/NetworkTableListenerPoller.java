// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import java.util.EnumSet;

/**
 * Topic change listener. This queues topic change events matching the specified mask. Code using
 * the listener must periodically call readQueue() to read the events.
 */
public final class NetworkTableListenerPoller implements AutoCloseable {
  /**
   * Construct a topic listener poller.
   *
   * @param inst Instance
   */
  public NetworkTableListenerPoller(NetworkTableInstance inst) {
    m_inst = inst;
    m_handle = NetworkTablesJNI.createListenerPoller(inst.getHandle());
  }

  /**
   * Start listening to topic changes for topics with names that start with any of the given
   * prefixes. This creates a corresponding internal subscriber with the lifetime of the listener.
   *
   * @param prefixes Topic name string prefixes
   * @param eventKinds set of event kinds to listen to
   * @return Listener handle
   */
  public int addListener(String[] prefixes, EnumSet<NetworkTableEvent.Kind> eventKinds) {
    return NetworkTablesJNI.addListener(m_handle, prefixes, eventKinds);
  }

  /**
   * Start listening to changes to a particular topic. This creates a corresponding internal
   * subscriber with the lifetime of the listener.
   *
   * @param topic Topic
   * @param eventKinds set of event kinds to listen to
   * @return Listener handle
   */
  public int addListener(Topic topic, EnumSet<NetworkTableEvent.Kind> eventKinds) {
    return NetworkTablesJNI.addListener(m_handle, topic.getHandle(), eventKinds);
  }

  /**
   * Start listening to topic changes on a subscriber. This does NOT keep the subscriber active.
   *
   * @param subscriber Subscriber
   * @param eventKinds set of event kinds to listen to
   * @return Listener handle
   */
  public int addListener(Subscriber subscriber, EnumSet<NetworkTableEvent.Kind> eventKinds) {
    return NetworkTablesJNI.addListener(m_handle, subscriber.getHandle(), eventKinds);
  }

  /**
   * Start listening to topic changes on a subscriber. This does NOT keep the subscriber active.
   *
   * @param subscriber Subscriber
   * @param eventKinds set of event kinds to listen to
   * @return Listener handle
   */
  public int addListener(MultiSubscriber subscriber, EnumSet<NetworkTableEvent.Kind> eventKinds) {
    return NetworkTablesJNI.addListener(m_handle, subscriber.getHandle(), eventKinds);
  }

  /**
   * Start listening to topic changes on an entry.
   *
   * @param entry Entry
   * @param eventKinds set of event kinds to listen to
   * @return Listener handle
   */
  public int addListener(NetworkTableEntry entry, EnumSet<NetworkTableEvent.Kind> eventKinds) {
    return NetworkTablesJNI.addListener(m_handle, entry.getHandle(), eventKinds);
  }

  /**
   * Add a connection listener. The callback function is called asynchronously on a separate thread,
   * so it's important to use synchronization or atomics when accessing any shared state from the
   * callback function.
   *
   * @param immediateNotify notify listener of all existing connections
   * @return Listener handle
   */
  public int addConnectionListener(boolean immediateNotify) {
    EnumSet<NetworkTableEvent.Kind> eventKinds = EnumSet.of(NetworkTableEvent.Kind.kConnection);
    if (immediateNotify) {
      eventKinds.add(NetworkTableEvent.Kind.kImmediate);
    }
    return NetworkTablesJNI.addListener(m_handle, m_inst.getHandle(), eventKinds);
  }

  /**
   * Add logger callback function. By default, log messages are sent to stderr; this function sends
   * log messages with the specified levels to the provided callback function instead. The callback
   * function will only be called for log messages with level greater than or equal to minLevel and
   * less than or equal to maxLevel; messages outside this range will be silently ignored.
   *
   * @param minLevel minimum log level
   * @param maxLevel maximum log level
   * @return Listener handle
   */
  public int addLogger(int minLevel, int maxLevel) {
    return NetworkTablesJNI.addLogger(m_handle, minLevel, maxLevel);
  }

  /**
   * Remove a listener.
   *
   * @param listener Listener handle
   */
  public void removeListener(int listener) {
    NetworkTablesJNI.removeListener(listener);
  }

  /**
   * Read topic notifications.
   *
   * @return Topic notifications since the previous call to readQueue()
   */
  public NetworkTableEvent[] readQueue() {
    return NetworkTablesJNI.readListenerQueue(m_inst, m_handle);
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      NetworkTablesJNI.destroyListenerPoller(m_handle);
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
