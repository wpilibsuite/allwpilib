// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <atomic>
#include <concepts>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/SmallVector.h>
#include <wpi/json_fwd.h>
#include <wpi/mutex.h>
#include <wpi/protobuf/Protobuf.h>

#include "networktables/NetworkTableInstance.h"
#include "networktables/Topic.h"
#include "ntcore_cpp.h"

namespace nt {

template <wpi::ProtobufSerializable T>
class ProtobufTopic;

/**
 * NetworkTables protobuf-encoded value subscriber.
 */
template <wpi::ProtobufSerializable T>
class ProtobufSubscriber : public Subscriber {
 public:
  using TopicType = ProtobufTopic<T>;
  using ValueType = T;
  using ParamType = const T&;
  using TimestampedValueType = Timestamped<T>;

  ProtobufSubscriber() = default;

  /**
   * Construct from a subscriber handle; recommended to use
   * ProtobufTopic::Subscribe() instead.
   *
   * @param handle Native handle
   * @param msg Protobuf message
   * @param defaultValue Default value
   */
  ProtobufSubscriber(NT_Subscriber handle, wpi::ProtobufMessage<T> msg,
                     T defaultValue)
      : Subscriber{handle},
        m_msg{std::move(msg)},
        m_defaultValue{std::move(defaultValue)} {}

  ProtobufSubscriber(const ProtobufSubscriber&) = delete;
  ProtobufSubscriber& operator=(const ProtobufSubscriber&) = delete;

  ProtobufSubscriber(ProtobufSubscriber&& rhs)
      : Subscriber{std::move(rhs)},
        m_msg{std::move(rhs.m_msg)},
        m_defaultValue{std::move(rhs.m_defaultValue)} {}

  ProtobufSubscriber& operator=(ProtobufSubscriber&& rhs) {
    Subscriber::operator=(std::move(rhs));
    m_msg = std::move(rhs.m_msg);
    m_defaultValue = std::move(rhs.m_defaultValue);
    return *this;
  }

  /**
   * Get the last published value.
   * If no value has been published or the value cannot be unpacked, returns the
   * stored default value.
   *
   * @return value
   */
  ValueType Get() const { return Get(m_defaultValue); }

  /**
   * Get the last published value.
   * If no value has been published or the value cannot be unpacked, returns the
   * passed defaultValue.
   *
   * @param defaultValue default value to return if no value has been published
   * @return value
   */
  ValueType Get(const T& defaultValue) const {
    return GetAtomic(defaultValue).value;
  }

  /**
   * Get the last published value, replacing the contents in place of an
   * existing object. If no value has been published or the value cannot be
   * unpacked, does not replace the contents and returns false.
   *
   * @param[out] out object to replace contents of
   * @return true if successful
   */
  bool GetInto(T* out) {
    wpi::SmallVector<uint8_t, 128> buf;
    TimestampedRawView view = ::nt::GetAtomicRaw(m_subHandle, buf, {});
    if (view.value.empty()) {
      return false;
    } else {
      std::scoped_lock lock{m_mutex};
      return m_msg.UnpackInto(out, view.value);
    }
  }

  /**
   * Get the last published value along with its timestamp
   * If no value has been published or the value cannot be unpacked, returns the
   * stored default value and a timestamp of 0.
   *
   * @return timestamped value
   */
  TimestampedValueType GetAtomic() const { return GetAtomic(m_defaultValue); }

  /**
   * Get the last published value along with its timestamp.
   * If no value has been published or the value cannot be unpacked, returns the
   * passed defaultValue and a timestamp of 0.
   *
   * @param defaultValue default value to return if no value has been published
   * @return timestamped value
   */
  TimestampedValueType GetAtomic(const T& defaultValue) const {
    wpi::SmallVector<uint8_t, 128> buf;
    TimestampedRawView view = ::nt::GetAtomicRaw(m_subHandle, buf, {});
    if (!view.value.empty()) {
      std::scoped_lock lock{m_mutex};
      if (auto optval = m_msg.Unpack(view.value)) {
        return {view.time, view.serverTime, *optval};
      }
    }
    return {0, 0, defaultValue};
  }

  /**
   * Get an array of all valid value changes since the last call to ReadQueue.
   * Also provides a timestamp for each value. Values that cannot be unpacked
   * are dropped.
   *
   * @note The "poll storage" subscribe option can be used to set the queue
   *     depth.
   *
   * @return Array of timestamped values; empty array if no valid new changes
   *     have been published since the previous call.
   */
  std::vector<TimestampedValueType> ReadQueue() {
    auto raw = ::nt::ReadQueueRaw(m_subHandle);
    std::vector<TimestampedValueType> rv;
    rv.reserve(raw.size());
    std::scoped_lock lock{m_mutex};
    for (auto&& r : raw) {
      if (auto optval = m_msg.Unpack(r.value)) {
        rv.emplace_back(r.time, r.serverTime, *optval);
      }
    }
    return rv;
  }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return ProtobufTopic<T>{::nt::GetTopicFromHandle(m_subHandle)};
  }

 private:
  mutable wpi::mutex m_mutex;
  mutable wpi::ProtobufMessage<T> m_msg;
  ValueType m_defaultValue;
};

/**
 * NetworkTables protobuf-encoded value publisher.
 */
template <wpi::ProtobufSerializable T>
class ProtobufPublisher : public Publisher {
 public:
  using TopicType = ProtobufTopic<T>;
  using ValueType = T;
  using ParamType = const T&;

  using TimestampedValueType = Timestamped<T>;

  ProtobufPublisher() = default;

  /**
   * Construct from a publisher handle; recommended to use
   * ProtobufTopic::Publish() instead.
   *
   * @param handle Native handle
   * @param msg Protobuf message
   */
  explicit ProtobufPublisher(NT_Publisher handle, wpi::ProtobufMessage<T> msg)
      : Publisher{handle}, m_msg{std::move(msg)} {}

  ProtobufPublisher(const ProtobufPublisher&) = delete;
  ProtobufPublisher& operator=(const ProtobufPublisher&) = delete;

  ProtobufPublisher(ProtobufPublisher&& rhs)
      : Publisher{std::move(rhs)},
        m_msg{std::move(rhs.m_msg)},
        m_schemaPublished{rhs.m_schemaPublished} {}

  ProtobufPublisher& operator=(ProtobufPublisher&& rhs) {
    Publisher::operator=(std::move(rhs));
    m_msg = std::move(rhs.m_msg);
    m_schemaPublished.store(
        rhs.m_schemaPublished.load(std::memory_order_relaxed),
        std::memory_order_relaxed);
    return *this;
  }

  /**
   * Publish a new value.
   *
   * @param value value to publish
   * @param time timestamp; 0 indicates current NT time should be used
   */
  void Set(const T& value, int64_t time = 0) {
    wpi::SmallVector<uint8_t, 128> buf;
    {
      std::scoped_lock lock{m_mutex};
      if (!m_schemaPublished.exchange(true, std::memory_order_relaxed)) {
        GetTopic().GetInstance().template AddProtobufSchema<T>(m_msg);
      }
      m_msg.Pack(buf, value);
    }
    ::nt::SetRaw(m_pubHandle, buf, time);
  }

  /**
   * Publish a default value.
   * On reconnect, a default value will never be used in preference to a
   * published value.
   *
   * @param value value
   */
  void SetDefault(const T& value) {
    wpi::SmallVector<uint8_t, 128> buf;
    {
      std::scoped_lock lock{m_mutex};
      if (!m_schemaPublished.exchange(true, std::memory_order_relaxed)) {
        GetTopic().GetInstance().template AddProtobufSchema<T>(m_msg);
      }
      m_msg.Pack(buf, value);
    }
    ::nt::SetDefaultRaw(m_pubHandle, buf);
  }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return ProtobufTopic<T>{::nt::GetTopicFromHandle(m_pubHandle)};
  }

 private:
  wpi::mutex m_mutex;
  wpi::ProtobufMessage<T> m_msg;
  std::atomic_bool m_schemaPublished{false};
};

/**
 * NetworkTables protobuf-encoded value entry.
 *
 * @note Unlike NetworkTableEntry, the entry goes away when this is destroyed.
 */
template <wpi::ProtobufSerializable T>
class ProtobufEntry final : public ProtobufSubscriber<T>,
                            public ProtobufPublisher<T> {
 public:
  using SubscriberType = ProtobufSubscriber<T>;
  using PublisherType = ProtobufPublisher<T>;
  using TopicType = ProtobufTopic<T>;
  using ValueType = T;
  using ParamType = const T&;

  using TimestampedValueType = Timestamped<T>;

  ProtobufEntry() = default;

  /**
   * Construct from an entry handle; recommended to use
   * ProtobufTopic::GetEntry() instead.
   *
   * @param handle Native handle
   * @param msg Protobuf message
   * @param defaultValue Default value
   */
  ProtobufEntry(NT_Entry handle, wpi::ProtobufMessage<T> msg, T defaultValue)
      : ProtobufSubscriber<T>{handle, std::move(msg), std::move(defaultValue)},
        ProtobufPublisher<T>{handle, wpi::ProtobufMessage<T>{}} {}

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return this->m_subHandle != 0; }

  /**
   * Gets the native handle for the entry.
   *
   * @return Native handle
   */
  NT_Entry GetHandle() const { return this->m_subHandle; }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return ProtobufTopic<T>{::nt::GetTopicFromHandle(this->m_subHandle)};
  }

  /**
   * Stops publishing the entry if it's published.
   */
  void Unpublish() { ::nt::Unpublish(this->m_pubHandle); }
};

/**
 * NetworkTables protobuf-encoded value topic.
 */
template <wpi::ProtobufSerializable T>
class ProtobufTopic final : public Topic {
 public:
  using SubscriberType = ProtobufSubscriber<T>;
  using PublisherType = ProtobufPublisher<T>;
  using EntryType = ProtobufEntry<T>;
  using ValueType = T;
  using ParamType = const T&;
  using TimestampedValueType = Timestamped<T>;

  ProtobufTopic() = default;

  /**
   * Construct from a topic handle; recommended to use
   * NetworkTableInstance::GetProtobufTopic() instead.
   *
   * @param handle Native handle
   */
  explicit ProtobufTopic(NT_Topic handle) : Topic{handle} {}

  /**
   * Construct from a generic topic.
   *
   * @param topic Topic
   */
  explicit ProtobufTopic(Topic topic) : Topic{topic} {}

  /**
   * Create a new subscriber to the topic.
   *
   * <p>The subscriber is only active as long as the returned object
   * is not destroyed.
   *
   * @note Subscribers that do not match the published data type do not return
   *     any values. To determine if the data type matches, use the appropriate
   *     Topic functions.
   *
   * @param defaultValue default value used when a default is not provided to a
   *        getter function
   * @param options subscribe options
   * @return subscriber
   */
  [[nodiscard]]
  SubscriberType Subscribe(
      T defaultValue, const PubSubOptions& options = kDefaultPubSubOptions) {
    wpi::ProtobufMessage<T> msg;
    auto typeString = msg.GetTypeString();
    return ProtobufSubscriber<T>{
        ::nt::Subscribe(m_handle, NT_RAW, typeString, options), std::move(msg),
        std::move(defaultValue)};
  }

  /**
   * Create a new publisher to the topic.
   *
   * The publisher is only active as long as the returned object
   * is not destroyed.
   *
   * @note It is not possible to publish two different data types to the same
   *     topic. Conflicts between publishers are typically resolved by the
   *     server on a first-come, first-served basis. Any published values that
   *     do not match the topic's data type are dropped (ignored). To determine
   *     if the data type matches, use the appropriate Topic functions.
   *
   * @param options publish options
   * @return publisher
   */
  [[nodiscard]]
  PublisherType Publish(const PubSubOptions& options = kDefaultPubSubOptions) {
    wpi::ProtobufMessage<T> msg;
    auto typeString = msg.GetTypeString();
    return ProtobufPublisher<T>{
        ::nt::Publish(m_handle, NT_RAW, typeString, options), std::move(msg)};
  }

  /**
   * Create a new publisher to the topic, with type string and initial
   * properties.
   *
   * The publisher is only active as long as the returned object
   * is not destroyed.
   *
   * @note It is not possible to publish two different data types to the same
   *     topic. Conflicts between publishers are typically resolved by the
   *     server on a first-come, first-served basis. Any published values that
   *     do not match the topic's data type are dropped (ignored). To determine
   *     if the data type matches, use the appropriate Topic functions.
   *
   * @param properties JSON properties
   * @param options publish options
   * @return publisher
   */
  [[nodiscard]]
  PublisherType PublishEx(
      const wpi::json& properties,
      const PubSubOptions& options = kDefaultPubSubOptions) {
    wpi::ProtobufMessage<T> msg;
    auto typeString = msg.GetTypeString();
    return ProtobufPublisher<T>{
        ::nt::PublishEx(m_handle, NT_RAW, typeString, properties, options),
        std::move(msg)};
  }

  /**
   * Create a new entry for the topic.
   *
   * Entries act as a combination of a subscriber and a weak publisher. The
   * subscriber is active as long as the entry is not destroyed. The publisher
   * is created when the entry is first written to, and remains active until
   * either Unpublish() is called or the entry is destroyed.
   *
   * @note It is not possible to use two different data types with the same
   *     topic. Conflicts between publishers are typically resolved by the
   *     server on a first-come, first-served basis. Any published values that
   *     do not match the topic's data type are dropped (ignored), and the entry
   *     will show no new values if the data type does not match. To determine
   *     if the data type matches, use the appropriate Topic functions.
   *
   * @param defaultValue default value used when a default is not provided to a
   *        getter function
   * @param options publish and/or subscribe options
   * @return entry
   */
  [[nodiscard]]
  EntryType GetEntry(T defaultValue,
                     const PubSubOptions& options = kDefaultPubSubOptions) {
    wpi::ProtobufMessage<T> msg;
    auto typeString = msg.GetTypeString();
    return ProtobufEntry<T>{
        ::nt::GetEntry(m_handle, NT_RAW, typeString, options), std::move(msg),
        std::move(defaultValue)};
  }
};

}  // namespace nt
