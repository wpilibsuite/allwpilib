// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <atomic>
#include <concepts>
#include <functional>
#include <span>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <wpi/SmallVector.h>
#include <wpi/json_fwd.h>
#include <wpi/struct/Struct.h>

#include "networktables/NetworkTableInstance.h"
#include "networktables/Topic.h"
#include "ntcore_cpp.h"

namespace nt {

template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructTopic;

/**
 * NetworkTables struct-encoded value subscriber.
 */
template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructSubscriber : public Subscriber {
  using S = wpi::Struct<T, I...>;

 public:
  using TopicType = StructTopic<T, I...>;
  using ValueType = T;
  using ParamType = const T&;
  using TimestampedValueType = Timestamped<T>;

  StructSubscriber() = default;

  /**
   * Construct from a subscriber handle; recommended to use
   * StructTopic::Subscribe() instead.
   *
   * @param handle Native handle
   * @param defaultValue Default value
   * @param info optional struct type info
   */
  StructSubscriber(NT_Subscriber handle, T defaultValue, I... info)
      : Subscriber{handle},
        m_defaultValue{std::move(defaultValue)},
        m_info{std::move(info)...} {}

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
    if (view.value.size() < std::apply(S::GetSize, m_info)) {
      return false;
    } else {
      std::apply(
          [&](const I&... info) {
            wpi::UnpackStructInto(out, view.value, info...);
          },
          m_info);
      return true;
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
    if (view.value.size() < std::apply(S::GetSize, m_info)) {
      return {0, 0, defaultValue};
    } else {
      return {
          view.time, view.serverTime,
          std::apply(
              [&](const I&... info) { return S::Unpack(view.value, info...); },
              m_info)};
    }
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
    for (auto&& r : raw) {
      if (r.value.size() < std::apply(S::GetSize, m_info)) {
        continue;
      } else {
        std::apply(
            [&](const I&... info) {
              rv.emplace_back(
                  r.time, r.serverTime,
                  S::Unpack(std::span<const uint8_t>(r.value), info...));
            },
            m_info);
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
    return std::apply(
        [&](const I&... info) {
          return StructTopic<T, I...>{::nt::GetTopicFromHandle(m_subHandle),
                                      info...};
        },
        m_info);
  }

 private:
  ValueType m_defaultValue;
  [[no_unique_address]] std::tuple<I...> m_info;
};

/**
 * NetworkTables struct-encoded value publisher.
 */
template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructPublisher : public Publisher {
  using S = wpi::Struct<T, I...>;

 public:
  using TopicType = StructTopic<T, I...>;
  using ValueType = T;
  using ParamType = const T&;

  using TimestampedValueType = Timestamped<T>;

  StructPublisher() = default;

  StructPublisher(const StructPublisher&) = delete;
  StructPublisher& operator=(const StructPublisher&) = delete;

  StructPublisher(StructPublisher&& rhs)
      : Publisher{std::move(rhs)},
        m_schemaPublished{
            rhs.m_schemaPublished.load(std::memory_order_relaxed)},
        m_info{std::move(rhs.m_info)} {}

  StructPublisher& operator=(StructPublisher&& rhs) {
    Publisher::operator=(std::move(rhs));
    m_schemaPublished.store(
        rhs.m_schemaPublished.load(std::memory_order_relaxed),
        std::memory_order_relaxed);
    m_info = std::move(rhs.m_info);
    return *this;
  }

  /**
   * Construct from a publisher handle; recommended to use
   * StructTopic::Publish() instead.
   *
   * @param handle Native handle
   * @param info optional struct type info
   */
  explicit StructPublisher(NT_Publisher handle, I... info)
      : Publisher{handle}, m_info{std::move(info)...} {}

  /**
   * Publish a new value.
   *
   * @param value value to publish
   * @param time timestamp; 0 indicates current NT time should be used
   */
  void Set(const T& value, int64_t time = 0) {
    if (!m_schemaPublished.exchange(true, std::memory_order_relaxed)) {
      std::apply(
          [&](const I&... info) {
            GetTopic().GetInstance().template AddStructSchema<T>(info...);
          },
          m_info);
    }
    if constexpr (sizeof...(I) == 0) {
      if constexpr (wpi::is_constexpr([] { S::GetSize(); })) {
        uint8_t buf[S::GetSize()];
        S::Pack(buf, value);
        ::nt::SetRaw(m_pubHandle, buf, time);
        return;
      }
    }
    wpi::SmallVector<uint8_t, 128> buf;
    buf.resize_for_overwrite(std::apply(S::GetSize, m_info));
    std::apply([&](const I&... info) { S::Pack(buf, value, info...); }, m_info);
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
    if (!m_schemaPublished.exchange(true, std::memory_order_relaxed)) {
      std::apply(
          [&](const I&... info) {
            GetTopic().GetInstance().template AddStructSchema<T>(info...);
          },
          m_info);
    }
    if constexpr (sizeof...(I) == 0) {
      if constexpr (wpi::is_constexpr([] { S::GetSize(); })) {
        uint8_t buf[S::GetSize()];
        S::Pack(buf, value);
        ::nt::SetDefaultRaw(m_pubHandle, buf);
        return;
      }
    }
    wpi::SmallVector<uint8_t, 128> buf;
    buf.resize_for_overwrite(std::apply(S::GetSize, m_info));
    std::apply([&](const I&... info) { S::Pack(buf, value, info...); }, m_info);
    ::nt::SetDefaultRaw(m_pubHandle, buf);
  }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return std::apply(
        [&](const I&... info) {
          return StructTopic<T, I...>{::nt::GetTopicFromHandle(m_pubHandle),
                                      info...};
        },
        m_info);
  }

 private:
  std::atomic_bool m_schemaPublished{false};
  [[no_unique_address]] std::tuple<I...> m_info;
};

/**
 * NetworkTables struct-encoded value entry.
 *
 * @note Unlike NetworkTableEntry, the entry goes away when this is destroyed.
 */
template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructEntry final : public StructSubscriber<T, I...>,
                          public StructPublisher<T, I...> {
 public:
  using SubscriberType = StructSubscriber<T, I...>;
  using PublisherType = StructPublisher<T, I...>;
  using TopicType = StructTopic<T, I...>;
  using ValueType = T;
  using ParamType = const T&;

  using TimestampedValueType = Timestamped<T>;

  StructEntry() = default;

  /**
   * Construct from an entry handle; recommended to use
   * StructTopic::GetEntry() instead.
   *
   * @param handle Native handle
   * @param defaultValue Default value
   * @param info optional struct type info
   */
  StructEntry(NT_Entry handle, T defaultValue, const I&... info)
      : StructSubscriber<T, I...>{handle, std::move(defaultValue), info...},
        StructPublisher<T, I...>{handle, info...} {}

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
  TopicType GetTopic() const { return StructSubscriber<T, I...>::GetTopic(); }

  /**
   * Stops publishing the entry if it's published.
   */
  void Unpublish() { ::nt::Unpublish(this->m_pubHandle); }
};

/**
 * NetworkTables struct-encoded value topic.
 */
template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructTopic final : public Topic {
 public:
  using SubscriberType = StructSubscriber<T, I...>;
  using PublisherType = StructPublisher<T, I...>;
  using EntryType = StructEntry<T, I...>;
  using ValueType = T;
  using ParamType = const T&;
  using TimestampedValueType = Timestamped<T>;

  StructTopic() = default;

  /**
   * Construct from a topic handle; recommended to use
   * NetworkTableInstance::GetStructTopic() instead.
   *
   * @param handle Native handle
   * @param info optional struct type info
   */
  explicit StructTopic(NT_Topic handle, I... info)
      : Topic{handle}, m_info{std::move(info)...} {}

  /**
   * Construct from a generic topic.
   *
   * @param topic Topic
   * @param info optional struct type info
   */
  explicit StructTopic(Topic topic, I... info)
      : Topic{topic}, m_info{std::move(info)...} {}

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
    return std::apply(
        [&](const I&... info) {
          return StructSubscriber<T, I...>{
              ::nt::Subscribe(m_handle, NT_RAW,
                              wpi::GetStructTypeString<T, I...>(info...),
                              options),
              std::move(defaultValue), info...};
        },
        m_info);
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
    return std::apply(
        [&](const I&... info) {
          return StructPublisher<T, I...>{
              ::nt::Publish(m_handle, NT_RAW,
                            wpi::GetStructTypeString<T, I...>(info...),
                            options),
              info...};
        },
        m_info);
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
    return std::apply(
        [&](const I&... info) {
          return StructPublisher<T, I...>{
              ::nt::PublishEx(m_handle, NT_RAW,
                              wpi::GetStructTypeString<T, I...>(info...),
                              properties, options),
              info...};
        },
        m_info);
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
    return std::apply(
        [&](const I&... info) {
          return StructEntry<T, I...>{
              ::nt::GetEntry(m_handle, NT_RAW,
                             wpi::GetStructTypeString<T, I...>(info...),
                             options),
              std::move(defaultValue), info...};
        },
        m_info);
  }

 private:
  [[no_unique_address]] std::tuple<I...> m_info;
};

}  // namespace nt
