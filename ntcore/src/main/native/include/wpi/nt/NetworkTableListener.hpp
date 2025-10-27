// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "networktables/MultiSubscriber.h"
#include "networktables/NetworkTableEntry.h"
#include "networktables/NetworkTableInstance.h"
#include "networktables/Topic.h"
#include "ntcore_cpp.h"

namespace nt {

class MultiSubscriber;
class NetworkTableEntry;
class NetworkTableInstance;
class Subscriber;
class Topic;

/**
 * Event listener. This calls back to a callback function when an event
 * matching the specified mask occurs. The callback function is called
 * asynchronously on a separate thread, so it's important to use synchronization
 * or atomics when accessing any shared state from the callback function.
 */
class NetworkTableListener final {
 public:
  NetworkTableListener() = default;

  /**
   * Create a listener for changes to topics with names that start with any of
   * the given prefixes. This creates a corresponding internal subscriber with
   * the lifetime of the listener.
   *
   * @param inst Instance
   * @param prefixes Topic name string prefixes
   * @param mask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener
   */
  static NetworkTableListener CreateListener(
      NetworkTableInstance inst, std::span<const std::string_view> prefixes,
      unsigned int mask, ListenerCallback listener) {
    return NetworkTableListener{::nt::AddListener(inst.GetHandle(), prefixes,
                                                  mask, std::move(listener))};
  }

  /**
   * Create a listener for changes on a particular topic. This creates a
   * corresponding internal subscriber with the lifetime of the listener.
   *
   * @param topic Topic
   * @param mask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener
   */
  static NetworkTableListener CreateListener(Topic topic, unsigned int mask,
                                             ListenerCallback listener) {
    return NetworkTableListener{
        nt::AddListener(topic.GetHandle(), mask, std::move(listener))};
  }

  /**
   * Create a listener for topic changes on a subscriber. This does NOT keep the
   * subscriber active.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener
   */
  static NetworkTableListener CreateListener(Subscriber& subscriber,
                                             unsigned int mask,
                                             ListenerCallback listener) {
    return NetworkTableListener{
        ::nt::AddListener(subscriber.GetHandle(), mask, std::move(listener))};
  }

  /**
   * Create a listener for topic changes on a subscriber. This does NOT keep the
   * subscriber active.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener
   */
  static NetworkTableListener CreateListener(MultiSubscriber& subscriber,
                                             unsigned int mask,
                                             ListenerCallback listener) {
    return NetworkTableListener{
        ::nt::AddListener(subscriber.GetHandle(), mask, std::move(listener))};
  }

  /**
   * Create a listener for topic changes on an entry.
   *
   * @param entry Entry
   * @param mask Bitmask of EventFlags values
   * @param listener Listener function
   * @return Listener
   */
  static NetworkTableListener CreateListener(NetworkTableEntry& entry,
                                             unsigned int mask,
                                             ListenerCallback listener) {
    return NetworkTableListener{
        ::nt::AddListener(entry.GetHandle(), mask, std::move(listener))};
  }

  /**
   * Create a connection listener.
   *
   * @param inst              instance
   * @param immediate_notify  notify listener of all existing connections
   * @param listener          listener function
   * @return Listener
   */
  static NetworkTableListener CreateConnectionListener(
      NetworkTableInstance inst, bool immediate_notify,
      ListenerCallback listener) {
    return NetworkTableListener{::nt::AddListener(
        inst.GetHandle(),
        NT_EVENT_CONNECTION | (immediate_notify ? NT_EVENT_IMMEDIATE : 0),
        std::move(listener))};
  }

  /**
   * Create a time synchronization listener.
   *
   * @param inst              instance
   * @param immediate_notify  notify listener of current time synchronization
   *                          value
   * @param listener          listener function
   * @return Listener
   */
  static NetworkTableListener CreateTimeSyncListener(
      NetworkTableInstance inst, bool immediate_notify,
      ListenerCallback listener) {
    return NetworkTableListener{::nt::AddListener(
        inst.GetHandle(),
        NT_EVENT_TIMESYNC | (immediate_notify ? NT_EVENT_IMMEDIATE : 0),
        std::move(listener))};
  }

  /**
   * Create a listener for log messages.  By default, log messages are sent to
   * stderr; this function sends log messages with the specified levels to the
   * provided callback function instead.  The callback function will only be
   * called for log messages with level greater than or equal to minLevel and
   * less than or equal to maxLevel; messages outside this range will be
   * silently ignored.
   *
   * @param inst        instance
   * @param minLevel    minimum log level
   * @param maxLevel    maximum log level
   * @param listener    listener function
   * @return Listener
   */
  static NetworkTableListener CreateLogger(NetworkTableInstance inst,
                                           unsigned int minLevel,
                                           unsigned int maxLevel,
                                           ListenerCallback listener) {
    return NetworkTableListener{::nt::AddLogger(inst.GetHandle(), minLevel,
                                                maxLevel, std::move(listener))};
  }

  NetworkTableListener(const NetworkTableListener&) = delete;
  NetworkTableListener& operator=(const NetworkTableListener&) = delete;

  NetworkTableListener(NetworkTableListener&& rhs) : m_handle(rhs.m_handle) {
    rhs.m_handle = 0;
  }

  NetworkTableListener& operator=(NetworkTableListener&& rhs) {
    if (m_handle != 0) {
      nt::RemoveListener(m_handle);
    }
    m_handle = rhs.m_handle;
    rhs.m_handle = 0;
    return *this;
  }

  ~NetworkTableListener() {
    if (m_handle != 0) {
      nt::RemoveListener(m_handle);
    }
  }

  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_Listener GetHandle() const { return m_handle; }

  /**
   * Wait for the listener queue to be empty. This is primarily useful for
   * deterministic testing. This blocks until either the listener queue is
   * empty (e.g. there are no more events that need to be passed along to
   * callbacks or poll queues) or the timeout expires.
   *
   * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or
   *                a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  bool WaitForQueue(double timeout) {
    if (m_handle != 0) {
      return nt::WaitForListenerQueue(m_handle, timeout);
    } else {
      return false;
    }
  }

 private:
  explicit NetworkTableListener(NT_Listener handle) : m_handle{handle} {}

  NT_Listener m_handle{0};
};

/**
 * Event polled listener. This queues events matching the specified mask. Code
 * using the listener must periodically call ReadQueue() to read the
 * events.
 */
class NetworkTableListenerPoller final {
 public:
  NetworkTableListenerPoller() = default;

  /**
   * Construct a listener poller.
   *
   * @param inst Instance
   */
  explicit NetworkTableListenerPoller(NetworkTableInstance inst)
      : m_handle(nt::CreateListenerPoller(inst.GetHandle())) {}

  NetworkTableListenerPoller(const NetworkTableListenerPoller&) = delete;
  NetworkTableListenerPoller& operator=(const NetworkTableListenerPoller&) =
      delete;

  NetworkTableListenerPoller(NetworkTableListenerPoller&& rhs)
      : m_handle(rhs.m_handle) {
    rhs.m_handle = 0;
  }

  NetworkTableListenerPoller& operator=(NetworkTableListenerPoller&& rhs) {
    if (m_handle != 0) {
      nt::DestroyListenerPoller(m_handle);
    }
    m_handle = rhs.m_handle;
    rhs.m_handle = 0;
    return *this;
  }

  ~NetworkTableListenerPoller() {
    if (m_handle != 0) {
      nt::DestroyListenerPoller(m_handle);
    }
  }

  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_ListenerPoller GetHandle() const { return m_handle; }

  /**
   * Start listening to topic changes for topics with names that start with any
   * of the given prefixes. This creates a corresponding internal subscriber
   * with the lifetime of the listener.
   *
   * @param prefixes Topic name string prefixes
   * @param mask Bitmask of EventFlags values
   * @return Listener handle
   */
  NT_Listener AddListener(std::span<const std::string_view> prefixes,
                          unsigned int mask) {
    return nt::AddPolledListener(m_handle, prefixes, mask);
  }

  /**
   * Start listening to changes to a particular topic. This creates a
   * corresponding internal subscriber with the lifetime of the listener.
   *
   * @param topic Topic
   * @param mask Bitmask of EventFlags values
   * @return Listener handle
   */
  NT_Listener AddListener(Topic topic, unsigned int mask) {
    return ::nt::AddPolledListener(m_handle, topic.GetHandle(), mask);
  }

  /**
   * Start listening to topic changes on a subscriber. This does NOT keep the
   * subscriber active.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of EventFlags values
   * @return Listener handle
   */
  NT_Listener AddListener(Subscriber& subscriber, unsigned int mask) {
    return ::nt::AddPolledListener(m_handle, subscriber.GetHandle(), mask);
  }

  /**
   * Start listening to topic changes on a subscriber. This does NOT keep the
   * subscriber active.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of EventFlags values
   * @return Listener handle
   */
  NT_Listener AddListener(MultiSubscriber& subscriber, unsigned int mask) {
    return ::nt::AddPolledListener(m_handle, subscriber.GetHandle(), mask);
  }

  /**
   * Start listening to topic changes on an entry.
   *
   * @param entry Entry
   * @param mask Bitmask of EventFlags values
   * @return Listener handle
   */
  NT_Listener AddListener(NetworkTableEntry& entry, unsigned int mask) {
    return ::nt::AddPolledListener(m_handle, entry.GetHandle(), mask);
  }

  /**
   * Add a connection listener. The callback function is called asynchronously
   * on a separate thread, so it's important to use synchronization or atomics
   * when accessing any shared state from the callback function.
   *
   * @param immediate_notify  notify listener of all existing connections
   * @return Listener handle
   */
  NT_Listener AddConnectionListener(bool immediate_notify) {
    return ::nt::AddPolledListener(
        m_handle, ::nt::GetInstanceFromHandle(m_handle),
        NT_EVENT_CONNECTION | (immediate_notify ? NT_EVENT_IMMEDIATE : 0));
  }

  /**
   * Add a time synchronization listener. The callback function is called
   * asynchronously on a separate thread, so it's important to use
   * synchronization or atomics when accessing any shared state from the
   * callback function.
   *
   * @param immediate_notify  notify listener of current time synchronization
   *                          value
   * @return Listener handle
   */
  NT_Listener AddTimeSyncListener(bool immediate_notify) {
    return ::nt::AddPolledListener(
        m_handle, ::nt::GetInstanceFromHandle(m_handle),
        NT_EVENT_TIMESYNC | (immediate_notify ? NT_EVENT_IMMEDIATE : 0));
  }

  /**
   * Add logger callback function.  By default, log messages are sent to stderr;
   * this function sends log messages with the specified levels to the provided
   * callback function instead.  The callback function will only be called for
   * log messages with level greater than or equal to minLevel and less than or
   * equal to maxLevel; messages outside this range will be silently ignored.
   *
   * @param minLevel    minimum log level
   * @param maxLevel    maximum log level
   * @return Listener handle
   */
  NT_Listener AddLogger(unsigned int minLevel, unsigned int maxLevel) {
    return ::nt::AddPolledLogger(m_handle, minLevel, maxLevel);
  }

  /**
   * Remove a listener.
   *
   * @param listener Listener handle
   */
  void RemoveListener(NT_Listener listener) { ::nt::RemoveListener(listener); }

  /**
   * Read events.
   *
   * @return Events since the previous call to ReadQueue()
   */
  std::vector<Event> ReadQueue() { return ::nt::ReadListenerQueue(m_handle); }

 private:
  NT_ListenerPoller m_handle{0};
};

}  // namespace nt
