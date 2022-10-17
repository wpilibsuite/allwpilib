// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <vector>

#include "ntcore_cpp.h"

namespace nt {

class MultiSubscriber;
class NetworkTableEntry;
class NetworkTableInstance;
class Subscriber;

/**
 * Flag values for use with value listeners.
 *
 * The flags are a bitmask and must be OR'ed together to indicate the
 * combination of events desired to be received.
 *
 * By default, notifications are only generated for remote changes occurring
 * after the listener is created. The constants kImmediate and kLocal are
 * modifiers that cause notifications to be generated at other times.
 */
struct ValueListenerFlags {
  ValueListenerFlags() = delete;

  /**
   * Initial listener addition.
   *
   * Set this flag to receive immediate notification of the current value.
   */
  static constexpr unsigned int kImmediate = NT_VALUE_NOTIFY_IMMEDIATE;

  /**
   * Changed locally.
   *
   * Set this flag to receive notification of both local changes and changes
   * coming from remote nodes. By default, notifications are only generated for
   * remote changes.
   */
  static constexpr unsigned int kLocal = NT_VALUE_NOTIFY_LOCAL;
};

/**
 * Value change listener. This calls back to a callback function when a value
 * change matching the specified mask occurs. The callback function is called
 * asynchronously on a separate thread, so it's important to use synchronization
 * or atomics when accessing any shared state from the callback function.
 */
class ValueListener final {
 public:
  ValueListener() = default;

  /**
   * Create a listener for value changes on a subscriber.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of ValueListenerFlags values
   * @param listener Listener function
   */
  ValueListener(const Subscriber& subscriber, unsigned int mask,
                std::function<void(const ValueNotification&)> listener);

  /**
   * Create a listener for value changes on a subscriber.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of ValueListenerFlags values
   * @param listener Listener function
   */
  ValueListener(const MultiSubscriber& subscriber, unsigned int mask,
                std::function<void(const ValueNotification&)> listener);

  /**
   * Create a listener for value changes on an entry.
   *
   * @param entry Entry
   * @param mask Bitmask of ValueListenerFlags values
   * @param listener Listener function
   */
  ValueListener(const NetworkTableEntry& entry, unsigned int mask,
                std::function<void(const ValueNotification&)> listener);

  /**
   * Create a listener for value changes on a subscriber/entry handle.
   *
   * @param subentry Subscriber/entry handle
   * @param mask Bitmask of ValueListenerFlags values
   * @param listener Listener function
   */
  ValueListener(NT_Handle subentry, unsigned int mask,
                std::function<void(const ValueNotification&)> listener);

  ValueListener(const ValueListener&) = delete;
  ValueListener& operator=(const ValueListener&) = delete;
  ValueListener(ValueListener&& rhs);
  ValueListener& operator=(ValueListener&& rhs);
  ~ValueListener();

  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_ValueListener GetHandle() const { return m_handle; }

 private:
  NT_ValueListener m_handle{0};
};

/**
 * Value change listener. This queues value change events matching the specified
 * mask. Code using the listener must periodically call readQueue() to read the
 * events.
 */
class ValueListenerPoller final {
 public:
  ValueListenerPoller();

  /**
   * Construct a value listener poller.
   *
   * @param inst Instance
   */
  explicit ValueListenerPoller(NetworkTableInstance inst);

  ValueListenerPoller(const ValueListenerPoller&) = delete;
  ValueListenerPoller& operator=(const ValueListenerPoller&) = delete;
  ValueListenerPoller(ValueListenerPoller&& rhs);
  ValueListenerPoller& operator=(ValueListenerPoller&& rhs);
  ~ValueListenerPoller();

  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_ValueListenerPoller GetHandle() const { return m_handle; }

  /**
   * Start listening to value changes on a subscriber.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of ValueListenerFlags values
   * @return Listener handle
   */
  NT_ValueListener Add(const Subscriber& subscriber, unsigned int mask);

  /**
   * Start listening to value changes on a subscriber.
   *
   * @param subscriber Subscriber
   * @param mask Bitmask of ValueListenerFlags values
   * @return Listener handle
   */
  NT_ValueListener Add(const MultiSubscriber& subscriber, unsigned int mask);

  /**
   * Start listening to value changes on an entry.
   *
   * @param entry Entry
   * @param mask Bitmask of ValueListenerFlags values
   * @return Listener handle
   */
  NT_ValueListener Add(const NetworkTableEntry& entry, unsigned int mask);

  /**
   * Start listening to value changes on a subscriber/entry handle.
   *
   * @param subentry Subscriber/entry handle
   * @param mask Bitmask of ValueListenerFlags values
   * @return Listener handle
   */
  NT_ValueListener Add(NT_Handle subentry, unsigned int mask);

  /**
   * Remove a listener.
   *
   * @param listener Listener handle
   */
  void Remove(NT_ValueListener listener);

  /**
   * Reads value listener queue (all value changes since last call).
   *
   * @param poller    poller handle
   * @return Array of value notifications.
   */
  std::vector<ValueNotification> ReadQueue();

 private:
  NT_ValueListenerPoller m_handle;
};

}  // namespace nt

#include "ValueListener.inc"
