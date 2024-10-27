// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string_view>
#include <vector>

#include <wpi/json.h>

#include "networktables/Topic.h"
#include "ntcore_cpp.h"

namespace nt {

template <typename T>
class UnitTopic;

/**
 * Timestamped unit.
 *
 * @tparam T unit type, e.g. units::meter_t
 */
template <typename T>
struct TimestampedUnit {
  TimestampedUnit() = default;
  TimestampedUnit(int64_t time, int64_t serverTime, T value)
      : time{time}, serverTime{serverTime}, value{value} {}

  /**
   * Time in local time base.
   */
  int64_t time = 0;

  /**
   * Time in server time base.  May be 0 or 1 for locally set values.
   */
  int64_t serverTime = 0;

  /**
   * Value.
   */
  T value = {};
};

/**
 * NetworkTables unit-typed subscriber.
 *
 * @tparam T unit type, e.g. units::meter_t
 */
template <typename T>
class UnitSubscriber : public Subscriber {
 public:
  using TopicType = UnitTopic<T>;
  using ValueType = T;
  using ParamType = T;
  using TimestampedValueType = TimestampedUnit<T>;

  UnitSubscriber() = default;

  /**
   * Construct from a subscriber handle; recommended to use
   * UnitTopic::Subscribe() instead.
   *
   * @param handle Native handle
   * @param defaultValue Default value
   */
  UnitSubscriber(NT_Subscriber handle, ParamType defaultValue)
      : Subscriber{handle}, m_defaultValue{defaultValue} {}

  /**
   * Get the last published value.
   * If no value has been published, returns the stored default value.
   *
   * @return value
   */
  ValueType Get() const { return Get(m_defaultValue); }

  /**
   * Get the last published value.
   * If no value has been published, returns the passed defaultValue.
   *
   * @param defaultValue default value to return if no value has been published
   * @return value
   */
  ValueType Get(ParamType defaultValue) const {
    return T{::nt::GetDouble(m_subHandle, defaultValue.value())};
  }

  /**
   * Get the last published value along with its timestamp
   * If no value has been published, returns the stored default value and a
   * timestamp of 0.
   *
   * @return timestamped value
   */
  TimestampedValueType GetAtomic() const { return GetAtomic(m_defaultValue); }

  /**
   * Get the last published value along with its timestamp.
   * If no value has been published, returns the passed defaultValue and a
   * timestamp of 0.
   *
   * @param defaultValue default value to return if no value has been published
   * @return timestamped value
   */
  TimestampedValueType GetAtomic(ParamType defaultValue) const {
    auto doubleVal = ::nt::GetAtomicDouble(m_subHandle, defaultValue.value());
    return {doubleVal.time, doubleVal.serverTime, doubleVal.value};
  }

  /**
   * Get an array of all value changes since the last call to ReadQueue.
   * Also provides a timestamp for each value.
   *
   * @note The "poll storage" subscribe option can be used to set the queue
   *     depth.
   *
   * @return Array of timestamped values; empty array if no new changes have
   *     been published since the previous call.
   */
  std::vector<TimestampedValueType> ReadQueue() {
    std::vector<TimestampedUnit<T>> vals;
    auto doubleVals = ::nt::ReadQueueDouble(m_subHandle);
    vals.reserve(doubleVals.size());
    for (auto&& val : doubleVals) {
      vals.emplace_back(val.time, val.serverTime, val.value);
    }
  }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return UnitTopic<T>{::nt::GetTopicFromHandle(m_subHandle)};
  }

 private:
  ValueType m_defaultValue;
};

/**
 * NetworkTables unit-typed publisher.
 *
 * @tparam T unit type, e.g. units::meter_t
 */
template <typename T>
class UnitPublisher : public Publisher {
 public:
  using TopicType = UnitTopic<T>;
  using ValueType = T;
  using ParamType = T;

  using TimestampedValueType = TimestampedUnit<T>;

  UnitPublisher() = default;

  /**
   * Construct from a publisher handle; recommended to use
   * UnitTopic::Publish() instead.
   *
   * @param handle Native handle
   */
  explicit UnitPublisher(NT_Publisher handle) : Publisher{handle} {}

  /**
   * Publish a new value.
   *
   * @param value value to publish
   * @param time timestamp; 0 indicates current NT time should be used
   */
  void Set(ParamType value, int64_t time = 0) {
    ::nt::SetDouble(m_pubHandle, value.value(), time);
  }

  /**
   * Publish a default value.
   * On reconnect, a default value will never be used in preference to a
   * published value.
   *
   * @param value value
   */
  void SetDefault(ParamType value) {
    ::nt::SetDefaultDouble(m_pubHandle, value.value());
  }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return UnitTopic<T>{::nt::GetTopicFromHandle(m_pubHandle)};
  }
};

/**
 * NetworkTables unit-typed entry.
 *
 * @note Unlike NetworkTableEntry, the entry goes away when this is destroyed.
 *
 * @tparam T unit type, e.g. units::meter_t
 */
template <typename T>
class UnitEntry final : public UnitSubscriber<T>, public UnitPublisher<T> {
 public:
  using SubscriberType = UnitSubscriber<T>;
  using PublisherType = UnitPublisher<T>;
  using TopicType = UnitTopic<T>;
  using ValueType = T;
  using ParamType = T;

  using TimestampedValueType = TimestampedUnit<T>;

  UnitEntry() = default;

  /**
   * Construct from an entry handle; recommended to use
   * UnitTopic::GetEntry() instead.
   *
   * @param handle Native handle
   * @param defaultValue Default value
   */
  UnitEntry(NT_Entry handle, ParamType defaultValue)
      : UnitSubscriber<T>{handle, defaultValue}, UnitPublisher<T>{handle} {}

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
    return UnitTopic<T>{::nt::GetTopicFromHandle(this->m_subHandle)};
  }

  /**
   * Stops publishing the entry if it's published.
   */
  void Unpublish() { ::nt::Unpublish(this->m_pubHandle); }
};

/**
 * NetworkTables unit-typed topic. Publishers publish the type name (e.g.
 * "meter") as the "unit" property. Type conversions are not performed--for
 * correct behavior the publisher and subscriber must use the same unit type,
 * but this can be checked at runtime using IsMatchingUnit().
 *
 * @tparam T unit type, e.g. units::meter_t
 */
template <typename T>
class UnitTopic final : public Topic {
 public:
  using SubscriberType = UnitSubscriber<T>;
  using PublisherType = UnitPublisher<T>;
  using EntryType = UnitEntry<T>;
  using ValueType = T;
  using ParamType = T;
  using TimestampedValueType = TimestampedUnit<T>;
  /** The default type string for this topic type. */
  static constexpr std::string_view kTypeString = "double";

  UnitTopic() = default;

  /**
   * Construct from a topic handle.
   *
   * @param handle Native handle
   */
  explicit UnitTopic(NT_Topic handle) : Topic{handle} {}

  /**
   * Construct from a generic topic.
   *
   * @param topic Topic
   */
  explicit UnitTopic(Topic topic) : Topic{topic} {}

  /**
   * Verify the topic has a matching unit type (if the topic is published).
   *
   * @return True if unit matches, false if not matching or topic not published.
   */
  bool IsMatchingUnit() const { return GetProperty("unit") == T{}.name(); }

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
      ParamType defaultValue,
      const PubSubOptions& options = kDefaultPubSubOptions) {
    return UnitSubscriber<T>{
        ::nt::Subscribe(m_handle, NT_DOUBLE, "double", options), defaultValue};
  }

  /**
   * Create a new subscriber to the topic, with specific type string.
   *
   * <p>The subscriber is only active as long as the returned object
   * is not destroyed.
   *
   * @note Subscribers that do not match the published data type do not return
   *     any values. To determine if the data type matches, use the appropriate
   *     Topic functions.
   *
   * @param typeString type string
   * @param defaultValue default value used when a default is not provided to a
   *        getter function
   * @param options subscribe options
   * @return subscriber
   */
  [[nodiscard]]
  SubscriberType SubscribeEx(
      std::string_view typeString, ParamType defaultValue,
      const PubSubOptions& options = kDefaultPubSubOptions) {
    return UnitSubscriber<T>{
        ::nt::Subscribe(m_handle, NT_DOUBLE, typeString, options),
        defaultValue};
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
    return UnitPublisher<T>{::nt::PublishEx(m_handle, NT_DOUBLE, "double",
                                            {{"unit", T{}.name()}}, options)};
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
   * @param typeString type string
   * @param properties JSON properties
   * @param options publish options
   * @return publisher
   */
  [[nodiscard]]
  PublisherType PublishEx(
      std::string_view typeString, const wpi::json& properties,
      const PubSubOptions& options = kDefaultPubSubOptions) {
    wpi::json props = properties;
    props["unit"] = T{}.name();
    return UnitPublisher<T>{
        ::nt::PublishEx(m_handle, NT_DOUBLE, typeString, props, options)};
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
  EntryType GetEntry(ParamType defaultValue,
                     const PubSubOptions& options = kDefaultPubSubOptions) {
    return UnitEntry<T>{::nt::GetEntry(m_handle, NT_DOUBLE, "double", options),
                        defaultValue};
  }

  /**
   * Create a new entry for the topic, with specific type string.
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
   * @param typeString type string
   * @param defaultValue default value used when a default is not provided to a
   *        getter function
   * @param options publish and/or subscribe options
   * @return entry
   */
  [[nodiscard]]
  EntryType GetEntryEx(std::string_view typeString, ParamType defaultValue,
                       const PubSubOptions& options = kDefaultPubSubOptions) {
    return UnitEntry<T>{
        ::nt::GetEntry(m_handle, NT_DOUBLE, typeString, options), defaultValue};
  }
};

}  // namespace nt
