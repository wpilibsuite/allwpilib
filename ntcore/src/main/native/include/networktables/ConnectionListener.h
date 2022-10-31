// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include "ntcore_cpp.h"

namespace nt {

class NetworkTableInstance;

/**
 * Connection listener. This calls back to a callback function when a connection
 * change occurs. The callback function is called asynchronously on a separate
 * thread, so it's important to use synchronization or atomics when accessing
 * any shared state from the callback function.
 */
class ConnectionListener final {
 public:
  ConnectionListener() = default;

  /**
   * Create a listener for connection changes.
   *
   * @param inst Instance
   * @param immediateNotify if notification should be immediately created for
   *                        existing connections
   * @param listener Listener function
   */
  ConnectionListener(
      NetworkTableInstance inst, bool immediateNotify,
      std::function<void(const ConnectionNotification&)> listener);

  ConnectionListener(const ConnectionListener&) = delete;
  ConnectionListener& operator=(const ConnectionListener&) = delete;
  ConnectionListener(ConnectionListener&& rhs);
  ConnectionListener& operator=(ConnectionListener&& rhs);
  ~ConnectionListener();

  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_ConnectionListener GetHandle() const { return m_handle; }

  /**
   * Wait for the connection listener queue to be empty. This is primarily
   * useful for deterministic testing. This blocks until either the connection
   * listener queue is empty (e.g. there are no more events that need to be
   * passed along to callbacks or poll queues) or the timeout expires.
   *
   * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or
   *                a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  bool WaitForQueue(double timeout);

 private:
  NT_ConnectionListener m_handle{0};
};

/**
 * A connection listener. This queues connection notifications. Code using
 * the listener must periodically call readQueue() to read the notifications.
 */
class ConnectionListenerPoller final {
 public:
  ConnectionListenerPoller() = default;

  /**
   * Construct a connection listener poller.
   *
   * @param inst Instance
   */
  explicit ConnectionListenerPoller(NetworkTableInstance inst);

  ConnectionListenerPoller(const ConnectionListenerPoller&) = delete;
  ConnectionListenerPoller& operator=(const ConnectionListenerPoller&) = delete;
  ConnectionListenerPoller(ConnectionListenerPoller&& rhs);
  ConnectionListenerPoller& operator=(ConnectionListenerPoller&& rhs);
  ~ConnectionListenerPoller();

  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_ConnectionListenerPoller GetHandle() const { return m_handle; }

  /**
   * Create a connection listener.
   *
   * @param immediateNotify if notification should be immediately created for
   * existing connections
   * @return Listener handle
   */
  NT_ConnectionListener Add(bool immediateNotify);

  /**
   * Remove a connection listener.
   *
   * @param listener Listener handle
   */
  void Remove(NT_ConnectionListener listener);

  /**
   * Read connection notifications.
   *
   * @return Connection notifications since the previous call to readQueue()
   */
  std::vector<ConnectionNotification> ReadQueue();

 private:
  NT_ConnectionListenerPoller m_handle{0};
};

}  // namespace nt

#include "ConnectionListener.inc"
