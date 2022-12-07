// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "networktables/Topic.h"

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
  explicit GenericSubscriber(NT_Subscriber handle);

  /**
   * Get the last published value.
   * If no value has been published, returns a value with unassigned type.
   *
   * @return value
   */
  ValueType Get() const;

  /**
   * Gets the entry's value as a boolean. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  bool GetBoolean(bool defaultValue) const;

  /**
   * Gets the entry's value as a integer. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  int64_t GetInteger(int64_t defaultValue) const;

  /**
   * Gets the entry's value as a float. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  float GetFloat(float defaultValue) const;

  /**
   * Gets the entry's value as a double. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  double GetDouble(double defaultValue) const;

  /**
   * Gets the entry's value as a string. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  std::string GetString(std::string_view defaultValue) const;

  /**
   * Gets the entry's value as a raw. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  std::vector<uint8_t> GetRaw(std::span<const uint8_t> defaultValue) const;

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
  std::vector<int> GetBooleanArray(std::span<const int> defaultValue) const;

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
      std::span<const int64_t> defaultValue) const;

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
  std::vector<float> GetFloatArray(std::span<const float> defaultValue) const;

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
      std::span<const double> defaultValue) const;

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
      std::span<const std::string> defaultValue) const;

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
  std::vector<TimestampedValueType> ReadQueue();

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const;
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
  explicit GenericPublisher(NT_Publisher handle);

  /**
   * Publish a new value.
   *
   * @param value value to publish
   */
  void Set(ParamType value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBoolean(bool value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetInteger(int64_t value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetFloat(float value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetDouble(double value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetString(std::string_view value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetRaw(std::span<const uint8_t> value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(std::span<const bool> value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(std::span<const int> value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetIntegerArray(std::span<const int64_t> value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetFloatArray(std::span<const float> value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetDoubleArray(std::span<const double> value, int64_t time = 0);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetStringArray(std::span<const std::string> value, int64_t time = 0);

  /**
   * Publish a default value.
   * On reconnect, a default value will never be used in preference to a
   * published value.
   *
   * @param value value
   */
  void SetDefault(ParamType value);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultBoolean(bool defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultInteger(int64_t defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultFloat(float defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultDouble(double defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultString(std::string_view defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultRaw(std::span<const uint8_t> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultBooleanArray(std::span<const int> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultIntegerArray(std::span<const int64_t> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultFloatArray(std::span<const float> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultDoubleArray(std::span<const double> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultStringArray(std::span<const std::string> defaultValue);

  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  TopicType GetTopic() const;
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
  explicit GenericEntry(NT_Entry handle);

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
  TopicType GetTopic() const;

  /**
   * Stops publishing the entry if it's published.
   */
  void Unpublish();
};

}  // namespace nt

#include "networktables/GenericEntry.inc"
