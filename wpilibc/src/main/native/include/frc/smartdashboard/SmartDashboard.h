// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableValue.h>

namespace wpi {
class Sendable;
}  // namespace wpi

namespace frc {

class SmartDashboard {
 public:
  SmartDashboard() = delete;

  static void init();

  /**
   * Determines whether the given key is in this table.
   *
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
  static bool ContainsKey(std::string_view key);

  /**
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  static std::vector<std::string> GetKeys(int types = 0);

  /**
   * Makes a key's value persistent through program restarts.
   *
   * @param key the key to make persistent
   */
  static void SetPersistent(std::string_view key);

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  static void ClearPersistent(std::string_view key);

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  static bool IsPersistent(std::string_view key);

  /**
   * Returns an NT Entry mapping to the specified key
   *
   * This is useful if an entry is used often, or is read and then modified.
   *
   * @param key the key
   * @return    the entry for the key
   */
  static nt::NetworkTableEntry GetEntry(std::string_view key);

  /**
   * Maps the specified key to the specified value in this table.
   *
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   *
   * In order for the value to appear in the dashboard, it must be registered
   * with SendableRegistry.  WPILib components do this automatically.
   *
   * @param key  the key
   * @param data the value
   */
  static void PutData(std::string_view key, wpi::Sendable* data);

  /**
   * Maps the specified key (where the key is the name of the Sendable)
   * to the specified value in this table.
   *
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   *
   * In order for the value to appear in the dashboard, it must be registered
   * with SendableRegistry.  WPILib components do this automatically.
   *
   * @param value the value
   */
  static void PutData(wpi::Sendable* value);

  /**
   * Returns the value at the specified key.
   *
   * @param keyName the key
   * @return the value
   */
  static wpi::Sendable* GetData(std::string_view keyName);

  /**
   * Maps the specified key to the specified value in this table.
   *
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   *
   * @param keyName the key
   * @param value   the value
   * @return        False if the table key already exists with a different type
   */
  static bool PutBoolean(std::string_view keyName, bool value);

  /**
   * Set the value in the table if key does not exist
   * @param key the key
   * @param defaultValue the value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  static bool SetDefaultBoolean(std::string_view key, bool defaultValue);

  /**
   * Returns the value at the specified key.
   *
   * If the key is not found, returns the default value.
   *
   * @param keyName the key
   * @param defaultValue the default value to return if key doesn't exist
   * @return the value
   */
  static bool GetBoolean(std::string_view keyName, bool defaultValue);

  /**
   * Maps the specified key to the specified value in this table.
   *
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   *
   * @param keyName the key
   * @param value   the value
   * @return        False if the table key already exists with a different type
   */
  static bool PutNumber(std::string_view keyName, double value);

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key          The key.
   * @param defaultValue The value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  static bool SetDefaultNumber(std::string_view key, double defaultValue);

  /**
   * Returns the value at the specified key.
   *
   * If the key is not found, returns the default value.
   *
   * @param keyName the key
   * @param defaultValue the default value to return if the key doesn't exist
   * @return the value
   */
  static double GetNumber(std::string_view keyName, double defaultValue);

  /**
   * Maps the specified key to the specified value in this table.
   *
   * The value can be retrieved by calling the get method with a key that is
   * equal to the original key.
   *
   * @param keyName the key
   * @param value   the value
   * @return        False if the table key already exists with a different type
   */
  static bool PutString(std::string_view keyName, std::string_view value);

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  static bool SetDefaultString(std::string_view key,
                               std::string_view defaultValue);

  /**
   * Returns the value at the specified key.
   *
   * If the key is not found, returns the default value.
   *
   * @param keyName the key
   * @param defaultValue the default value to return if the key doesn't exist
   * @return the value
   */
  static std::string GetString(std::string_view keyName,
                               std::string_view defaultValue);

  /**
   * Put a boolean array in the table.
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   *
   * @note The array must be of int's rather than of bool's because
   *       std::vector<bool> is special-cased in C++. 0 is false, any
   *       non-zero value is true.
   */
  static bool PutBooleanArray(std::string_view key, std::span<const int> value);

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue the value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  static bool SetDefaultBooleanArray(std::string_view key,
                                     std::span<const int> defaultValue);

  /**
   * Returns the boolean array the key maps to.
   *
   * If the key does not exist or is of different type, it will return the
   * default value.
   *
   * @param key          The key to look up.
   * @param defaultValue The value to be returned if no value is found.
   * @return the value associated with the given key or the given default value
   *         if there is no value associated with the key
   *
   * @note This makes a copy of the array. If the overhead of this is a concern,
   *       use GetValue() instead.
   *
   * @note The returned array is std::vector<int> instead of std::vector<bool>
   *       because std::vector<bool> is special-cased in C++. 0 is false, any
   *       non-zero value is true.
   */
  static std::vector<int> GetBooleanArray(std::string_view key,
                                          std::span<const int> defaultValue);

  /**
   * Put a number array in the table.
   *
   * @param key   The key to be assigned to.
   * @param value The value that will be assigned.
   * @return False if the table key already exists with a different type
   */
  static bool PutNumberArray(std::string_view key,
                             std::span<const double> value);

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key          The key.
   * @param defaultValue The value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  static bool SetDefaultNumberArray(std::string_view key,
                                    std::span<const double> defaultValue);

  /**
   * Returns the number array the key maps to.
   *
   * If the key does not exist or is of different type, it will return the
   * default value.
   *
   * @param key The key to look up.
   * @param defaultValue The value to be returned if no value is found.
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the array. If the overhead of this is a concern,
   *       use GetValue() instead.
   */
  static std::vector<double> GetNumberArray(
      std::string_view key, std::span<const double> defaultValue);

  /**
   * Put a string array in the table.
   *
   * @param key   The key to be assigned to.
   * @param value The value that will be assigned.
   * @return False if the table key already exists with a different type
   */
  static bool PutStringArray(std::string_view key,
                             std::span<const std::string> value);

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key          The key.
   * @param defaultValue The value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  static bool SetDefaultStringArray(std::string_view key,
                                    std::span<const std::string> defaultValue);

  /**
   * Returns the string array the key maps to.
   *
   * If the key does not exist or is of different type, it will return the
   * default value.
   *
   * @param key          The key to look up.
   * @param defaultValue The value to be returned if no value is found.
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the array. If the overhead of this is a concern,
   *       use GetValue() instead.
   */
  static std::vector<std::string> GetStringArray(
      std::string_view key, std::span<const std::string> defaultValue);

  /**
   * Put a raw value (byte array) in the table.
   *
   * @param key   The key to be assigned to.
   * @param value The value that will be assigned.
   * @return False if the table key already exists with a different type
   */
  static bool PutRaw(std::string_view key, std::span<const uint8_t> value);

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key          The key.
   * @param defaultValue The value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  static bool SetDefaultRaw(std::string_view key,
                            std::span<const uint8_t> defaultValue);

  /**
   * Returns the raw value (byte array) the key maps to.
   *
   * If the key does not exist or is of different type, it will return the
   * default value.
   *
   * @param key          The key to look up.
   * @param defaultValue The value to be returned if no value is found.
   * @return the value associated with the given key or the given default value
   *         if there is no value associated with the key
   *
   * @note This makes a copy of the raw contents. If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  static std::vector<uint8_t> GetRaw(std::string_view key,
                                     std::span<const uint8_t> defaultValue);

  /**
   * Maps the specified key to the specified complex value (such as an array) in
   * this table.
   *
   * The value can be retrieved by calling the RetrieveValue method with a key
   * that is equal to the original key.
   *
   * @param keyName the key
   * @param value   the value
   * @return        False if the table key already exists with a different type
   */
  static bool PutValue(std::string_view keyName, const nt::Value& value);

  /**
   * Set the value in the table if key does not exist.
   *
   * @param key the key
   * @param defaultValue The value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  static bool SetDefaultValue(std::string_view key,
                              const nt::Value& defaultValue);

  /**
   * Retrieves the complex value (such as an array) in this table into the
   * complex data object.
   *
   * @param keyName the key
   */
  static nt::Value GetValue(std::string_view keyName);

  /**
   * Posts a task from a listener to the ListenerExecutor, so that it can be run
   * synchronously from the main loop on the next call to updateValues().
   *
   * @param task The task to run synchronously from the main thread.
   */
  static void PostListenerTask(std::function<void()> task);

  /**
   * Puts all sendable data to the dashboard.
   */
  static void UpdateValues();
};

}  // namespace frc
