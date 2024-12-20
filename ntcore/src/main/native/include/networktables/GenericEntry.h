// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "networktables/Topic.h"
#include "ntcore_cpp.h"

namespace nt {

class Value;

/**
 * NetworkTables generic subscriber.
 */
class GenericSubscriber : public Subscriber {
 public:
  using TopicType = Topic;
  using ValueType = Value;
  using ParamType = const Value&;
  using TimestampedValueType = Value;

  GenericSubscriber() = default;

  /**
   * Construct from a subscriber handle; recommended to use
   * Topic::GenericSubscribe() instead.
   *
   * @param handle Native handle
   */
  explicit GenericSubscriber(NT_Subscriber handle) : Subscriber{handle} {}

  /**
   * Get the last published value.
   * If no value has been published, returns a value with unassigned type.
   *
   * @return value
   */
  ValueType Get() const { return ::nt::GetEntryValue(m_subHandle); }

  /**
   * Gets the entry's value as a boolean. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  bool GetBoolean(bool defaultValue) const {
    return ::nt::GetBoolean(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a integer. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  int64_t GetInteger(int64_t defaultValue) const {
    return ::nt::GetInteger(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a float. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  float GetFloat(float defaultValue) const {
    return ::nt::GetFloat(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a double. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  double GetDouble(double defaultValue) const {
    return ::nt::GetDouble(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a string. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  std::string GetString(std::string_view defaultValue) const {
    return ::nt::GetString(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a raw. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  std::vector<uint8_t> GetRaw(std::span<const uint8_t> defaultValue) const {
    return ::nt::GetRaw(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a boolean array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   *
   * @note The returned array is std::vector<int> instead of std::vector<bool>
   *       because std::vector<bool> is special-cased in C++.  0 is false, any
   *       non-zero value is true.
   */
  std::vector<int> GetBooleanArray(std::span<const int> defaultValue) const {
    return ::nt::GetBooleanArray(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a integer array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  std::vector<int64_t> GetIntegerArray(
      std::span<const int64_t> defaultValue) const {
    return ::nt::GetIntegerArray(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a float array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  std::vector<float> GetFloatArray(std::span<const float> defaultValue) const {
    return ::nt::GetFloatArray(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a double array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  std::vector<double> GetDoubleArray(
      std::span<const double> defaultValue) const {
    return ::nt::GetDoubleArray(m_subHandle, defaultValue);
  }

  /**
   * Gets the entry's value as a string array. If the entry does not exist
   * or is of different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  std::vector<std::string> GetStringArray(
      std::span<const std::string> defaultValue) const {
    return ::nt::GetStringArray(m_subHandle, defaultValue);
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
    return ::nt::ReadQueueValue(m_subHandle);
  }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return Topic{::nt::GetTopicFromHandle(m_subHandle)};
  }
};

/**
 * NetworkTables generic publisher.
 */
class GenericPublisher : public Publisher {
 public:
  using TopicType = Topic;
  using ValueType = Value;
  using ParamType = const Value&;
  using TimestampedValueType = Value;

  GenericPublisher() = default;

  /**
   * Construct from a publisher handle; recommended to use
   * Topic::GenericPublish() instead.
   *
   * @param handle Native handle
   */
  explicit GenericPublisher(NT_Publisher handle) : Publisher{handle} {}

  /**
   * Publish a new value.
   *
   * @param value value to publish
   */
  void Set(ParamType value) { ::nt::SetEntryValue(m_pubHandle, value); }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBoolean(bool value, int64_t time = 0) {
    return nt::SetBoolean(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetInteger(int64_t value, int64_t time = 0) {
    return nt::SetInteger(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetFloat(float value, int64_t time = 0) {
    return nt::SetFloat(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetDouble(double value, int64_t time = 0) {
    return nt::SetDouble(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetString(std::string_view value, int64_t time = 0) {
    return nt::SetString(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetRaw(std::span<const uint8_t> value, int64_t time = 0) {
    return nt::SetRaw(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(std::span<const bool> value, int64_t time = 0) {
    return SetEntryValue(m_pubHandle, Value::MakeBooleanArray(value, time));
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(std::span<const int> value, int64_t time = 0) {
    return nt::SetBooleanArray(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetIntegerArray(std::span<const int64_t> value, int64_t time = 0) {
    return nt::SetIntegerArray(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetFloatArray(std::span<const float> value, int64_t time = 0) {
    return nt::SetFloatArray(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetDoubleArray(std::span<const double> value, int64_t time = 0) {
    return nt::SetDoubleArray(m_pubHandle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetStringArray(std::span<const std::string> value, int64_t time = 0) {
    return nt::SetStringArray(m_pubHandle, value, time);
  }

  /**
   * Publish a default value.
   * On reconnect, a default value will never be used in preference to a
   * published value.
   *
   * @param value value
   */
  void SetDefault(ParamType value) {
    ::nt::SetDefaultEntryValue(m_pubHandle, value);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultBoolean(bool defaultValue) {
    return nt::SetDefaultBoolean(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultInteger(int64_t defaultValue) {
    return nt::SetDefaultInteger(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultFloat(float defaultValue) {
    return nt::SetDefaultFloat(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultDouble(double defaultValue) {
    return nt::SetDefaultDouble(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultString(std::string_view defaultValue) {
    return nt::SetDefaultString(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultRaw(std::span<const uint8_t> defaultValue) {
    return nt::SetDefaultRaw(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultBooleanArray(std::span<const int> defaultValue) {
    return nt::SetDefaultBooleanArray(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultIntegerArray(std::span<const int64_t> defaultValue) {
    return nt::SetDefaultIntegerArray(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultFloatArray(std::span<const float> defaultValue) {
    return nt::SetDefaultFloatArray(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultDoubleArray(std::span<const double> defaultValue) {
    return nt::SetDefaultDoubleArray(m_pubHandle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultStringArray(std::span<const std::string> defaultValue) {
    return nt::SetDefaultStringArray(m_pubHandle, defaultValue);
  }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return Topic{::nt::GetTopicFromHandle(m_pubHandle)};
  }
};

/**
 * NetworkTables generic entry.
 *
 * @note Unlike NetworkTableEntry, the entry goes away when this is destroyed.
 */
class GenericEntry final : public GenericSubscriber, public GenericPublisher {
 public:
  using SubscriberType = GenericSubscriber;
  using PublisherType = GenericPublisher;
  using TopicType = Topic;
  using ValueType = Value;
  using ParamType = const Value&;
  using TimestampedValueType = Value;

  GenericEntry() = default;

  /**
   * Construct from an entry handle; recommended to use
   * RawTopic::GetEntry() instead.
   *
   * @param handle Native handle
   */
  explicit GenericEntry(NT_Entry handle)
      : GenericSubscriber{handle}, GenericPublisher{handle} {}

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return m_subHandle != 0; }

  /**
   * Gets the native handle for the entry.
   *
   * @return Native handle
   */
  NT_Entry GetHandle() const { return m_subHandle; }

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const {
    return Topic{::nt::GetTopicFromHandle(m_subHandle)};
  }

  /**
   * Stops publishing the entry if it's published.
   */
  void Unpublish() { ::nt::Unpublish(m_pubHandle); }
};

}  // namespace nt
