// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/nt/NetworkTableType.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/nt/ntcore_cpp.hpp"

namespace wpi::nt {

class NetworkTableInstance;
class Topic;

/**
 * NetworkTables Entry
 *
 * @note For backwards compatibility, the NetworkTableEntry destructor does not
 *       release the entry.
 *
 * @ingroup ntcore_cpp_api
 */
class NetworkTableEntry final {
 public:
  /**
   * Construct invalid instance.
   */
  NetworkTableEntry() = default;

  /**
   * Construct from native handle.
   *
   * @param handle Native handle
   */
  explicit NetworkTableEntry(NT_Entry handle) : m_handle{handle} {}

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  explicit operator bool() const { return m_handle != 0; }

  /**
   * Gets the native handle for the entry.
   *
   * @return Native handle
   */
  NT_Entry GetHandle() const { return m_handle; }

  /**
   * Gets the instance for the entry.
   *
   * @return Instance
   */
  NetworkTableInstance GetInstance() const;

  /**
   * Determines if the entry currently exists.
   *
   * @return True if the entry exists, false otherwise.
   */
  bool Exists() const { return GetEntryType(m_handle) != NT_UNASSIGNED; }

  /**
   * Gets the name of the entry (the key).
   *
   * @return the entry's name
   */
  std::string GetName() const { return GetEntryName(m_handle); }

  /**
   * Gets the type of the entry.
   *
   * @return the entry's type
   */
  NetworkTableType GetType() const {
    return static_cast<NetworkTableType>(GetEntryType(m_handle));
  }

  /**
   * Gets the last time the entry's value was changed.
   *
   * @return Entry last change time
   */
  int64_t GetLastChange() const { return GetEntryLastChange(m_handle); }

  /**
   * Gets the entry's value. If the entry does not exist, returns an empty
   * value.
   *
   * @return the entry's value or an empty value if it does not exist.
   */
  Value GetValue() const { return GetEntryValue(m_handle); }

  /**
   * Gets the entry's value as a boolean. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  bool GetBoolean(bool defaultValue) const {
    return wpi::nt::GetBoolean(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a integer. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  int64_t GetInteger(int64_t defaultValue) const {
    return wpi::nt::GetInteger(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a float. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  float GetFloat(float defaultValue) const {
    return wpi::nt::GetFloat(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a double. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  double GetDouble(double defaultValue) const {
    return wpi::nt::GetDouble(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a string. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  std::string GetString(std::string_view defaultValue) const {
    return wpi::nt::GetString(m_handle, defaultValue);
  }

  /**
   * Gets the entry's value as a raw. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  std::vector<uint8_t> GetRaw(std::span<const uint8_t> defaultValue) const {
    return wpi::nt::GetRaw(m_handle, defaultValue);
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
    return wpi::nt::GetBooleanArray(m_handle, defaultValue);
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
    return wpi::nt::GetIntegerArray(m_handle, defaultValue);
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
    return wpi::nt::GetFloatArray(m_handle, defaultValue);
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
    return wpi::nt::GetDoubleArray(m_handle, defaultValue);
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
    return wpi::nt::GetStringArray(m_handle, defaultValue);
  }

  /**
   * Get an array of all value changes since the last call to ReadQueue.
   *
   * The "poll storage" subscribe option can be used to set the queue depth.
   *
   * @return Array of values; empty array if no new changes have been
   *     published since the previous call.
   */
  std::vector<NetworkTableValue> ReadQueue() {
    return wpi::nt::ReadQueueValue(m_handle);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultValue(const Value& defaultValue) {
    return SetDefaultEntryValue(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultBoolean(bool defaultValue) {
    return wpi::nt::SetDefaultBoolean(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultInteger(int64_t defaultValue) {
    return wpi::nt::SetDefaultInteger(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultFloat(float defaultValue) {
    return wpi::nt::SetDefaultFloat(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultDouble(double defaultValue) {
    return wpi::nt::SetDefaultDouble(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultString(std::string_view defaultValue) {
    return wpi::nt::SetDefaultString(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultRaw(std::span<const uint8_t> defaultValue) {
    return wpi::nt::SetDefaultRaw(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultBooleanArray(std::span<const int> defaultValue) {
    return wpi::nt::SetDefaultBooleanArray(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultIntegerArray(std::span<const int64_t> defaultValue) {
    return wpi::nt::SetDefaultIntegerArray(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultFloatArray(std::span<const float> defaultValue) {
    return wpi::nt::SetDefaultFloatArray(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultDoubleArray(std::span<const double> defaultValue) {
    return wpi::nt::SetDefaultDoubleArray(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return True if the entry did not already have a value, otherwise False
   */
  bool SetDefaultStringArray(std::span<const std::string> defaultValue) {
    return wpi::nt::SetDefaultStringArray(m_handle, defaultValue);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetValue(const Value& value) { return SetEntryValue(m_handle, value); }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBoolean(bool value, int64_t time = 0) {
    return wpi::nt::SetBoolean(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetInteger(int64_t value, int64_t time = 0) {
    return wpi::nt::SetInteger(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetFloat(float value, int64_t time = 0) {
    return wpi::nt::SetFloat(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetDouble(double value, int64_t time = 0) {
    return wpi::nt::SetDouble(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetString(std::string_view value, int64_t time = 0) {
    return wpi::nt::SetString(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetRaw(std::span<const uint8_t> value, int64_t time = 0) {
    return wpi::nt::SetRaw(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(std::span<const bool> value, int64_t time = 0) {
    return SetEntryValue(m_handle, Value::MakeBooleanArray(value, time));
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(std::span<const int> value, int64_t time = 0) {
    return wpi::nt::SetBooleanArray(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetIntegerArray(std::span<const int64_t> value, int64_t time = 0) {
    return wpi::nt::SetIntegerArray(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetFloatArray(std::span<const float> value, int64_t time = 0) {
    return wpi::nt::SetFloatArray(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetDoubleArray(std::span<const double> value, int64_t time = 0) {
    return wpi::nt::SetDoubleArray(m_handle, value, time);
  }

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @param time the timestamp to set (0 = wpi::nt::Now())
   * @return False if the entry exists with a different type
   */
  bool SetStringArray(std::span<const std::string> value, int64_t time = 0) {
    return wpi::nt::SetStringArray(m_handle, value, time);
  }

  /**
   * Make value persistent through program restarts.
   */
  void SetPersistent() {
    wpi::nt::SetTopicPersistent(wpi::nt::GetTopicFromHandle(m_handle), true);
  }

  /**
   * Stop making value persistent through program restarts.
   */
  void ClearPersistent() {
    wpi::nt::SetTopicPersistent(wpi::nt::GetTopicFromHandle(m_handle), false);
  }

  /**
   * Returns whether the value is persistent through program restarts.
   *
   * @return True if the value is persistent.
   */
  bool IsPersistent() const {
    return wpi::nt::GetTopicPersistent(wpi::nt::GetTopicFromHandle(m_handle));
  }

  /**
   * Stops publishing the entry if it's been published.
   */
  void Unpublish() { return wpi::nt::Unpublish(m_handle); }

  /**
   * Gets the entry's topic.
   *
   * @return Topic
   */
  Topic GetTopic() const;

  /**
   * Equality operator.  Returns true if both instances refer to the same
   * native handle.
   */
  bool operator==(const NetworkTableEntry&) const = default;

 protected:
  /* Native handle */
  NT_Entry m_handle{0};
};

}  // namespace wpi::nt
