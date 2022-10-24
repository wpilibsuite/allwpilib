// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <span>
#include <string_view>
#include <vector>

#include "ntcore_cpp.h"

namespace nt {

class MultiSubscriber;
class NetworkTableEntry;
class NetworkTableInstance;
class Subscriber;
class Topic;

/**
 * Flag values for use with topic listeners.
 *
 * The flags are a bitmask and must be OR'ed together to indicate the
 * combination of events desired to be received.
 *
 * The constants kPublish, kUnpublish, and kProperties represent different
 * events that can occur to entries.
 *
 * @ingroup ntcore_cpp_api
 */
struct TopicListenerFlags {
  TopicListenerFlags() = delete;

  /**
   * Initial listener addition.
   * Set this flag to receive immediate notification of entries matching the
   * flag criteria (generally only useful when combined with kPublish).
   */
  static constexpr unsigned int kImmediate = NT_TOPIC_NOTIFY_IMMEDIATE;

  /**
   * Newly published topic.
   *
   * Set this flag to receive a notification when a topic is initially
   * published.
   */
  static constexpr unsigned int kPublish = NT_TOPIC_NOTIFY_PUBLISH;

  /**
   * Topic has no more publishers.
   *
   * Set this flag to receive a notification when a topic has no more
   * publishers.
   */
  static constexpr unsigned int kUnpublish = NT_TOPIC_NOTIFY_UNPUBLISH;

  /**
   * Topic's properties changed.
   *
   * Set this flag to receive a notification when an topic's properties change.
   */
  static constexpr unsigned int kProperties = NT_TOPIC_NOTIFY_PROPERTIES;
};

/**
 * Topic change listener. This calls back to a callback function when a topic
 * change matching the specified mask occurs. The callback function is called
 * asynchronously on a separate thread, so it's important to use synchronization
 * or atomics when accessing any shared state from the callback function.
 */
class TopicListener final {
 public:
  TopicListener() = default;

  /**
   * Create a listener for changes to topics with names that start with any of
   * the given prefixes.
   *
   * @param inst Instance
   * @param prefixes Topic name string prefixes
   * @param mask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  TopicListener(NetworkTableInstance inst,
                std::span<const std::string_view> prefixes, unsigned int mask,
                std::function<void(const TopicNotification&)> listener);

  /**
   * Create a listener for changes on a particular topic.
   *
   * @param topic Topic
   * @param mask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  TopicListener(Topic topic, unsigned int mask,
                std::function<void(const TopicNotification&)> listener);

  /**
   * Create a listener for topic changes on a subscriber. This does NOT keep the
   * subscriber active.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  TopicListener(Subscriber& subscriber, unsigned int mask,
                std::function<void(const TopicNotification&)> listener);

  /**
   * Create a listener for topic changes on a subscriber. This does NOT keep the
   * subscriber active.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  TopicListener(MultiSubscriber& subscriber, unsigned int mask,
                std::function<void(const TopicNotification&)> listener);

  /**
   * Create a listener for topic changes on an entry.
   *
   * @param entry Entry
   * @param mask Bitmask of TopicListenerFlags values
   * @param listener Listener function
   */
  TopicListener(NetworkTableEntry& entry, unsigned int mask,
                std::function<void(const TopicNotification&)> listener);

  TopicListener(const TopicListener&) = delete;
  TopicListener& operator=(const TopicListener&) = delete;
  TopicListener(TopicListener&& rhs);
  TopicListener& operator=(TopicListener&& rhs);
  ~TopicListener();

  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_TopicListener GetHandle() const { return m_handle; }

  /**
   * Wait for the topic listener queue to be empty. This is primarily useful for
   * deterministic testing. This blocks until either the topic listener queue is
   * empty (e.g. there are no more events that need to be passed along to
   * callbacks or poll queues) or the timeout expires.
   *
   * @param timeout timeout, in seconds. Set to 0 for non-blocking behavior, or
   *                a negative value to block indefinitely
   * @return False if timed out, otherwise true.
   */
  bool WaitForQueue(double timeout);

 private:
  NT_TopicListener m_handle{0};
};

/**
 * Topic change listener. This queues topic change events matching the specified
 * mask. Code using the listener must periodically call readQueue() to read the
 * events.
 */
class TopicListenerPoller final {
 public:
  TopicListenerPoller() = default;

  /**
   * Construct a topic listener poller.
   *
   * @param inst Instance
   */
  explicit TopicListenerPoller(NetworkTableInstance inst);

  TopicListenerPoller(const TopicListenerPoller&) = delete;
  TopicListenerPoller& operator=(const TopicListenerPoller&) = delete;
  TopicListenerPoller(TopicListenerPoller&& rhs);
  TopicListenerPoller& operator=(TopicListenerPoller&& rhs);
  ~TopicListenerPoller();

  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_TopicListenerPoller GetHandle() const { return m_handle; }

  /**
   * Start listening to changes to a particular topic.
   *
   * @param prefixes Topic name string prefixes
   * @param mask Bitmask of TopicListenerFlags values
   * @return Listener handle
   */
  NT_TopicListener Add(std::span<const std::string_view> prefixes,
                       unsigned int mask);

  /**
   * Start listening to topic changes for topics with names that start with any
   * of the given prefixes.
   *
   * @param topic Topic
   * @param mask Bitmask of TopicListenerFlags values
   * @return Listener handle
   */
  NT_TopicListener Add(Topic topic, unsigned int mask);

  /**
   * Start listening to topic changes on a subscriber. This does NOT keep the
   * subscriber active.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of TopicListenerFlags values
   * @return Listener handle
   */
  NT_TopicListener Add(Subscriber& subscriber, unsigned int mask);

  /**
   * Start listening to topic changes on a subscriber. This does NOT keep the
   * subscriber active.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of TopicListenerFlags values
   * @return Listener handle
   */
  NT_TopicListener Add(MultiSubscriber& subscriber, unsigned int mask);

  /**
   * Start listening to topic changes on an entry.
   *
   * @param entry Entry
   * @param mask Bitmask of TopicListenerFlags values
   * @return Listener handle
   */
  NT_TopicListener Add(NetworkTableEntry& entry, unsigned int mask);

  /**
   * Remove a listener.
   *
   * @param listener Listener handle
   */
  void Remove(NT_TopicListener listener);

  /**
   * Read topic notifications.
   *
   * @return Topic notifications since the previous call to readQueue()
   */
  std::vector<TopicNotification> ReadQueue();

 private:
  NT_TopicListenerPoller m_handle{0};
};

}  // namespace nt

#include "TopicListener.inc"
