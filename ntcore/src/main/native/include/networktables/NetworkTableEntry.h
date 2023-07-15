// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "networktables/NetworkTableType.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace nt {

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
   * Flag values (as returned by GetFlags()).
   * @deprecated Use IsPersistent() instead.
   */
  enum Flags { kPersistent = NT_PERSISTENT };

  /**
   * Construct invalid instance.
   */
  NetworkTableEntry();

  /**
   * Construct from native handle.
   *
   * @param handle Native handle
   */
  explicit NetworkTableEntry(NT_Entry handle);

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
  NT_Entry GetHandle() const;

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
  bool Exists() const;

  /**
   * Gets the name of the entry (the key).
   *
   * @return the entry's name
   */
  std::string GetName() const;

  /**
   * Gets the type of the entry.
   *
   * @return the entry's type
   */
  NetworkTableType GetType() const;

  /**
   * Returns the flags.
   *
   * @return the flags (bitmask)
   * @deprecated Use IsPersistent() or topic properties instead
   */
  [[deprecated("Use IsPersistent() or topic properties instead")]]
  unsigned int GetFlags() const;

  /**
   * Gets the last time the entry's value was changed.
   *
   * @return Entry last change time
   */
  int64_t GetLastChange() const;

  /**
   * Gets the entry's value. If the entry does not exist, returns an empty
   * value.
   *
   * @return the entry's value or an empty value if it does not exist.
   */
  Value GetValue() const;

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
   *
   * The "poll storage" subscribe option can be used to set the queue depth.
   *
   * @return Array of values; empty array if no new changes have been
   *     published since the previous call.
   */
  std::vector<NetworkTableValue> ReadQueue();

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultValue(const Value& defaultValue);

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
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetValue(const Value& value);

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
   * Sets flags.
   *
   * @param flags the flags to set (bitmask)
   * @deprecated Use SetPersistent() or topic properties instead
   */
  [[deprecated("Use SetPersistent() or topic properties instead")]]
  void SetFlags(unsigned int flags);

  /**
   * Clears flags.
   *
   * @param flags the flags to clear (bitmask)
   * @deprecated Use SetPersistent() or topic properties instead
   */
  [[deprecated("Use SetPersistent() or topic properties instead")]]
  void ClearFlags(unsigned int flags);

  /**
   * Make value persistent through program restarts.
   */
  void SetPersistent();

  /**
   * Stop making value persistent through program restarts.
   */
  void ClearPersistent();

  /**
   * Returns whether the value is persistent through program restarts.
   *
   * @return True if the value is persistent.
   */
  bool IsPersistent() const;

  /**
   * Stops publishing the entry if it's been published.
   */
  void Unpublish();

  /**
   * Deletes the entry.
   * @deprecated Use Unpublish() instead.
   */
  [[deprecated("Use Unpublish() instead")]]
  void Delete();

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

}  // namespace nt

#include "networktables/NetworkTableEntry.inc"
