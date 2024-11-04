// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string_view>

#include "networktables/NetworkTableInstance.h"
#include "ntcore_cpp.h"

namespace nt {

/**
 * Subscribe to multiple topics based on one or more topic name prefixes. Can be
 * used in combination with NetworkTableListenerPoller to listen for value
 * changes across all matching topics.
 */
class MultiSubscriber final {
 public:
  MultiSubscriber() = default;

  /**
   * Create a multiple subscriber.
   *
   * @param inst instance
   * @param prefixes topic name prefixes
   * @param options subscriber options
   */
  MultiSubscriber(NetworkTableInstance inst,
                  std::span<const std::string_view> prefixes,
                  const PubSubOptions& options = kDefaultPubSubOptions)
      : m_handle{::nt::SubscribeMultiple(inst.GetHandle(), prefixes, options)} {
  }

  MultiSubscriber(const MultiSubscriber&) = delete;
  MultiSubscriber& operator=(const MultiSubscriber&) = delete;

  MultiSubscriber(MultiSubscriber&& rhs) : m_handle{rhs.m_handle} {
    rhs.m_handle = 0;
  }

  MultiSubscriber& operator=(MultiSubscriber&& rhs) {
    if (m_handle != 0) {
      ::nt::UnsubscribeMultiple(m_handle);
    }
    m_handle = rhs.m_handle;
    rhs.m_handle = 0;
    return *this;
  }

  ~MultiSubscriber() {
    if (m_handle != 0) {
      ::nt::UnsubscribeMultiple(m_handle);
    }
  }

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  NT_MultiSubscriber GetHandle() const { return m_handle; }

 private:
  NT_MultiSubscriber m_handle{0};
};

}  // namespace nt
