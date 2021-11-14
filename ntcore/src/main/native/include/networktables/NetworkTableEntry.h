// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_NETWORKTABLEENTRY_H_
#define NTCORE_NETWORKTABLES_NETWORKTABLEENTRY_H_

#include <stdint.h>

#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/span.h>

#include "networktables/NetworkTableType.h"
#include "networktables/NetworkTableValue.h"
#include "networktables/RpcCall.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace nt {

class NetworkTableInstance;

/**
 * NetworkTables Entry
 * @ingroup ntcore_cpp_api
 */
class NetworkTableEntry final {
 public:
  /**
   * Flag values (as returned by GetFlags()).
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
   */
  unsigned int GetFlags() const;

  /**
   * Gets the last time the entry's value was changed.
   *
   * @return Entry last change time
   */
  uint64_t GetLastChange() const;

  /**
   * Gets combined information about the entry.
   *
   * @return Entry information
   */
  EntryInfo GetInfo() const;

  /**
   * Gets the entry's value. If the entry does not exist, returns nullptr.
   *
   * @return the entry's value or nullptr if it does not exist.
   */
  std::shared_ptr<Value> GetValue() const;

  /**
   * Gets the entry's value as a boolean. If the entry does not exist or is of
   * different type, it will return the default value.
   *
   * @param defaultValue the value to be returned if no value is found
   * @return the entry's value or the given default value
   */
  bool GetBoolean(bool defaultValue) const;

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
  std::string GetRaw(std::string_view defaultValue) const;

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
  std::vector<int> GetBooleanArray(wpi::span<const int> defaultValue) const;

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
  std::vector<int> GetBooleanArray(
      std::initializer_list<int> defaultValue) const;

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
      wpi::span<const double> defaultValue) const;

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
      std::initializer_list<double> defaultValue) const;

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
      wpi::span<const std::string> defaultValue) const;

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
      std::initializer_list<std::string> defaultValue) const;

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param value the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultValue(std::shared_ptr<Value> value);

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
  bool SetDefaultRaw(std::string_view defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultBooleanArray(wpi::span<const int> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultBooleanArray(std::initializer_list<int> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultDoubleArray(wpi::span<const double> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultDoubleArray(std::initializer_list<double> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultStringArray(wpi::span<const std::string> defaultValue);

  /**
   * Sets the entry's value if it does not exist.
   *
   * @param defaultValue the default value to set
   * @return False if the entry exists with a different type
   */
  bool SetDefaultStringArray(std::initializer_list<std::string> defaultValue);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetValue(std::shared_ptr<Value> value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetBoolean(bool value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetDouble(double value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetString(std::string_view value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetRaw(std::string_view value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(wpi::span<const bool> value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(std::initializer_list<bool> value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(wpi::span<const int> value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetBooleanArray(std::initializer_list<int> value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetDoubleArray(wpi::span<const double> value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetDoubleArray(std::initializer_list<double> value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetStringArray(wpi::span<const std::string> value);

  /**
   * Sets the entry's value.
   *
   * @param value the value to set
   * @return False if the entry exists with a different type
   */
  bool SetStringArray(std::initializer_list<std::string> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetValue(std::shared_ptr<Value> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetBoolean(bool value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetDouble(double value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetString(std::string_view value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetRaw(std::string_view value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetBooleanArray(wpi::span<const bool> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetBooleanArray(std::initializer_list<bool> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetBooleanArray(wpi::span<const int> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetBooleanArray(std::initializer_list<int> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetDoubleArray(wpi::span<const double> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetDoubleArray(std::initializer_list<double> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetStringArray(wpi::span<const std::string> value);

  /**
   * Sets the entry's value.  If the value is of different type, the type is
   * changed to match the new value.
   *
   * @param value the value to set
   */
  void ForceSetStringArray(std::initializer_list<std::string> value);

  /**
   * Sets flags.
   *
   * @param flags the flags to set (bitmask)
   */
  void SetFlags(unsigned int flags);

  /**
   * Clears flags.
   *
   * @param flags the flags to clear (bitmask)
   */
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
   * Deletes the entry.
   */
  void Delete();

  /**
   * Create a callback-based RPC entry point.  Only valid to use on the server.
   * The callback function will be called when the RPC is called.
   * This function creates RPC version 0 definitions (raw data in and out).
   *
   * @param callback  callback function
   */
  void CreateRpc(std::function<void(const RpcAnswer& answer)> callback);

  /**
   * Create a polled RPC entry point.  Only valid to use on the server.
   * The caller is responsible for calling NetworkTableInstance::PollRpc()
   * to poll for servicing incoming RPC calls.
   * This function creates RPC version 0 definitions (raw data in and out).
   */
  void CreatePolledRpc();

  /**
   * Call a RPC function.  May be used on either the client or server.
   * This function is non-blocking.  Either RpcCall::GetResult() or
   * RpcCall::CancelResult() must be called on the return value to either
   * get or ignore the result of the call.
   *
   * @param params      parameter
   * @return RPC call object.
   */
  RpcCall CallRpc(std::string_view params);

  /**
   * Add a listener for changes to this entry.
   *
   * @param callback          listener to add
   * @param flags             NotifyKind bitmask
   * @return Listener handle
   */
  NT_EntryListener AddListener(
      std::function<void(const EntryNotification& event)> callback,
      unsigned int flags) const;

  /**
   * Remove an entry listener.
   *
   * @param entry_listener Listener handle to remove
   */
  void RemoveListener(NT_EntryListener entry_listener);

  /**
   * Equality operator.  Returns true if both instances refer to the same
   * native handle.
   */
  bool operator==(const NetworkTableEntry& oth) const {
    return m_handle == oth.m_handle;
  }

  /** Inequality operator. */
  bool operator!=(const NetworkTableEntry& oth) const {
    return !(*this == oth);
  }

 protected:
  /* Native handle */
  NT_Entry m_handle{0};
};

}  // namespace nt

#include "networktables/NetworkTableEntry.inc"

#endif  // NTCORE_NETWORKTABLES_NETWORKTABLEENTRY_H_
