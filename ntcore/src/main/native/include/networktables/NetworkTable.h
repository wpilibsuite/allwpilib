// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/StringMap.h>
#include <wpi/mutex.h>
#include <wpi/protobuf/Protobuf.h>
#include <wpi/struct/Struct.h>

#include "networktables/NetworkTableEntry.h"
#include "networktables/Topic.h"
#include "ntcore_c.h"

namespace nt {

class BooleanArrayTopic;
class BooleanTopic;
class DoubleArrayTopic;
class DoubleTopic;
class FloatArrayTopic;
class FloatTopic;
class IntegerArrayTopic;
class IntegerTopic;
class NetworkTableInstance;
template <wpi::ProtobufSerializable T>
class ProtobufTopic;
class RawTopic;
class StringArrayTopic;
class StringTopic;
template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructArrayTopic;
template <typename T, typename... I>
  requires wpi::StructSerializable<T, I...>
class StructTopic;
class Topic;

/**
 * @defgroup ntcore_cpp_api ntcore C++ object-oriented API
 *
 * Recommended interface for C++, identical to Java API.
 */

/**
 * A network table that knows its subtable path.
 * @ingroup ntcore_cpp_api
 */
class NetworkTable final {
 private:
  NT_Inst m_inst;
  std::string m_path;
  mutable wpi::mutex m_mutex;
  mutable wpi::StringMap<NT_Entry> m_entries;

  struct private_init {};
  friend class NetworkTableInstance;

 public:
  /**
   * Gets the "base name" of a key. For example, "/foo/bar" becomes "bar".
   * If the key has a trailing slash, returns an empty string.
   *
   * @param key key
   * @return base name
   */
  static std::string_view BasenameKey(std::string_view key);

  /**
   * Normalizes an network table key to contain no consecutive slashes and
   * optionally start with a leading slash. For example:
   *
   * <pre><code>
   * normalizeKey("/foo/bar", true)  == "/foo/bar"
   * normalizeKey("foo/bar", true)   == "/foo/bar"
   * normalizeKey("/foo/bar", false) == "foo/bar"
   * normalizeKey("foo//bar", false) == "foo/bar"
   * </code></pre>
   *
   * @param key              the key to normalize
   * @param withLeadingSlash whether or not the normalized key should begin
   *                         with a leading slash
   * @return normalized key
   */
  static std::string NormalizeKey(std::string_view key,
                                  bool withLeadingSlash = true);

  static std::string_view NormalizeKey(std::string_view key,
                                       wpi::SmallVectorImpl<char>& buf,
                                       bool withLeadingSlash = true);

  /**
   * Gets a list of the names of all the super tables of a given key. For
   * example, the key "/foo/bar/baz" has a hierarchy of "/", "/foo",
   * "/foo/bar", and "/foo/bar/baz".
   *
   * @param key the key
   * @return List of super tables
   */
  static std::vector<std::string> GetHierarchy(std::string_view key);

  /**
   * Constructor.  Use NetworkTableInstance::GetTable() or GetSubTable()
   * instead.
   */
  NetworkTable(NT_Inst inst, std::string_view path, const private_init&);
  ~NetworkTable();

  /**
   * Gets the instance for the table.
   *
   * @return Instance
   */
  NetworkTableInstance GetInstance() const;

  /**
   * The path separator for sub-tables and keys
   */
  static constexpr char PATH_SEPARATOR_CHAR = '/';

  /**
   * Gets the entry for a subkey.
   *
   * @param key the key name
   * @return Network table entry.
   */
  NetworkTableEntry GetEntry(std::string_view key) const;

  /**
   * Get (generic) topic.
   *
   * @param name topic name
   * @return Topic
   */
  Topic GetTopic(std::string_view name) const;

  /**
   * Get boolean topic.
   *
   * @param name topic name
   * @return BooleanTopic
   */
  BooleanTopic GetBooleanTopic(std::string_view name) const;

  /**
   * Get integer topic.
   *
   * @param name topic name
   * @return IntegerTopic
   */
  IntegerTopic GetIntegerTopic(std::string_view name) const;

  /**
   * Get float topic.
   *
   * @param name topic name
   * @return FloatTopic
   */
  FloatTopic GetFloatTopic(std::string_view name) const;

  /**
   * Get double topic.
   *
   * @param name topic name
   * @return DoubleTopic
   */
  DoubleTopic GetDoubleTopic(std::string_view name) const;

  /**
   * Get String topic.
   *
   * @param name topic name
   * @return StringTopic
   */
  StringTopic GetStringTopic(std::string_view name) const;

  /**
   * Get raw topic.
   *
   * @param name topic name
   * @return BooleanArrayTopic
   */
  RawTopic GetRawTopic(std::string_view name) const;

  /**
   * Get boolean[] topic.
   *
   * @param name topic name
   * @return BooleanArrayTopic
   */
  BooleanArrayTopic GetBooleanArrayTopic(std::string_view name) const;

  /**
   * Get integer[] topic.
   *
   * @param name topic name
   * @return IntegerArrayTopic
   */
  IntegerArrayTopic GetIntegerArrayTopic(std::string_view name) const;

  /**
   * Get float[] topic.
   *
   * @param name topic name
   * @return FloatArrayTopic
   */
  FloatArrayTopic GetFloatArrayTopic(std::string_view name) const;

  /**
   * Get double[] topic.
   *
   * @param name topic name
   * @return DoubleArrayTopic
   */
  DoubleArrayTopic GetDoubleArrayTopic(std::string_view name) const;

  /**
   * Get String[] topic.
   *
   * @param name topic name
   * @return StringArrayTopic
   */
  StringArrayTopic GetStringArrayTopic(std::string_view name) const;

  /**
   * Gets a protobuf serialized value topic.
   *
   * @param name topic name
   * @return Topic
   */
  template <wpi::ProtobufSerializable T>
  ProtobufTopic<T> GetProtobufTopic(std::string_view name) const {
    return ProtobufTopic<T>{GetTopic(name)};
  }

  /**
   * Gets a raw struct serialized value topic.
   *
   * @param name topic name
   * @param info optional struct type info
   * @return Topic
   */
  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  StructTopic<T, I...> GetStructTopic(std::string_view name, I... info) const {
    return StructTopic<T, I...>{GetTopic(name), std::move(info)...};
  }

  /**
   * Gets a raw struct serialized array topic.
   *
   * @param name topic name
   * @param info optional struct type info
   * @return Topic
   */
  template <typename T, typename... I>
    requires wpi::StructSerializable<T, I...>
  StructArrayTopic<T, I...> GetStructArrayTopic(std::string_view name,
                                                I... info) const {
    return StructArrayTopic<T, I...>{GetTopic(name), std::move(info)...};
  }

  /**
   * Returns the table at the specified key. If there is no table at the
   * specified key, it will create a new table
   *
   * @param key the key name
   * @return the networktable to be returned
   */
  std::shared_ptr<NetworkTable> GetSubTable(std::string_view key) const;

  /**
   * Determines whether the given key is in this table.
   *
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
  bool ContainsKey(std::string_view key) const;

  /**
   * Determines whether there exists a non-empty subtable for this key
   * in this table.
   *
   * @param key the key to search for
   * @return true if there is a subtable with the key which contains at least
   * one key/subtable of its own
   */
  bool ContainsSubTable(std::string_view key) const;

  /**
   * Gets topic information for all keys in the table (not including
   * sub-tables).
   *
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return topic information for keys currently in the table
   */
  std::vector<TopicInfo> GetTopicInfo(int types = 0) const;

  /**
   * Gets all topics in the table (not including sub-tables).
   *
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return topic for keys currently in the table
   */
  std::vector<Topic> GetTopics(int types = 0) const;

  /**
   * Gets all keys in the table (not including sub-tables).
   *
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  std::vector<std::string> GetKeys(int types = 0) const;

  /**
   * Gets the names of all subtables in the table.
   *
   * @return subtables currently in the table
   */
  std::vector<std::string> GetSubTables() const;

  /**
   * Makes a key's value persistent through program restarts.
   *
   * @param key the key to make persistent
   */
  void SetPersistent(std::string_view key);

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  void ClearPersistent(std::string_view key);

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  bool IsPersistent(std::string_view key) const;

  /**
   * Put a number in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutNumber(std::string_view key, double value);

  /**
   * Set Default Entry Value
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  bool SetDefaultNumber(std::string_view key, double defaultValue);

  /**
   * Gets the number associated with the given name.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  double GetNumber(std::string_view key, double defaultValue) const;

  /**
   * Put a string in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutString(std::string_view key, std::string_view value);

  /**
   * Set Default Entry Value
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  bool SetDefaultString(std::string_view key, std::string_view defaultValue);

  /**
   * Gets the string associated with the given name. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  std::string GetString(std::string_view key,
                        std::string_view defaultValue) const;

  /**
   * Put a boolean in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutBoolean(std::string_view key, bool value);

  /**
   * Set Default Entry Value
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  bool SetDefaultBoolean(std::string_view key, bool defaultValue);

  /**
   * Gets the boolean associated with the given name. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  bool GetBoolean(std::string_view key, bool defaultValue) const;

  /**
   * Put a boolean array in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   *
   * @note The array must be of int's rather than of bool's because
   *       std::vector<bool> is special-cased in C++.  0 is false, any
   *       non-zero value is true.
   */
  bool PutBooleanArray(std::string_view key, std::span<const int> value);

  /**
   * Set Default Entry Value
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return True if the table key did not already exist, otherwise False
   */
  bool SetDefaultBooleanArray(std::string_view key,
                              std::span<const int> defaultValue);

  /**
   * Returns the boolean array the key maps to. If the key does not exist or is
   * of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   *
   * @note The returned array is std::vector<int> instead of std::vector<bool>
   *       because std::vector<bool> is special-cased in C++.  0 is false, any
   *       non-zero value is true.
   */
  std::vector<int> GetBooleanArray(std::string_view key,
                                   std::span<const int> defaultValue) const;

  /**
   * Put a number array in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutNumberArray(std::string_view key, std::span<const double> value);

  /**
   * Set Default Entry Value
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  bool SetDefaultNumberArray(std::string_view key,
                             std::span<const double> defaultValue);

  /**
   * Returns the number array the key maps to. If the key does not exist or is
   * of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  std::vector<double> GetNumberArray(
      std::string_view key, std::span<const double> defaultValue) const;

  /**
   * Put a string array in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutStringArray(std::string_view key, std::span<const std::string> value);

  /**
   * Set Default Entry Value
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns True if the table key did not already exist, otherwise False
   */
  bool SetDefaultStringArray(std::string_view key,
                             std::span<const std::string> defaultValue);

  /**
   * Returns the string array the key maps to. If the key does not exist or is
   * of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  std::vector<std::string> GetStringArray(
      std::string_view key, std::span<const std::string> defaultValue) const;

  /**
   * Put a raw value (byte array) in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutRaw(std::string_view key, std::span<const uint8_t> value);

  /**
   * Set Default Entry Value
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return True if the table key did not already exist, otherwise False
   */
  bool SetDefaultRaw(std::string_view key,
                     std::span<const uint8_t> defaultValue);

  /**
   * Returns the raw value (byte array) the key maps to. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the raw contents.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  std::vector<uint8_t> GetRaw(std::string_view key,
                              std::span<const uint8_t> defaultValue) const;

  /**
   * Put a value in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutValue(std::string_view key, const Value& value);

  /**
   * Set Default Entry Value.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return True if the table key did not already exist, otherwise False
   */
  bool SetDefaultValue(std::string_view key, const Value& defaultValue);

  /**
   * Gets the value associated with a key as an object
   *
   * @param key the key of the value to look up
   * @return the value associated with the given key, or nullptr if the key
   * does not exist
   */
  Value GetValue(std::string_view key) const;

  /**
   * Gets the full path of this table.  Does not include the trailing "/".
   *
   * @return The path (e.g "", "/foo").
   */
  std::string_view GetPath() const;

  /**
   * Called when an event occurs on a topic in a {@link NetworkTable}.
   *
   * @param table the table the topic exists in
   * @param key the key associated with the topic that changed
   * @param event the event
   */
  using TableEventListener = std::function<void(
      NetworkTable* table, std::string_view key, const Event& event)>;

  /**
   * Listen to topics only within this table.
   *
   * @param eventMask Bitmask of EventFlags values
   * @param listener listener to add
   * @return Listener handle
   */
  NT_Listener AddListener(int eventMask, TableEventListener listener);

  /**
   * Listen to a single key.
   *
   * @param key the key name
   * @param eventMask Bitmask of EventFlags values
   * @param listener listener to add
   * @return Listener handle
   */
  NT_Listener AddListener(std::string_view key, int eventMask,
                          TableEventListener listener);

  /**
   * Called when a new table is created within a NetworkTable.
   *
   * @param parent the parent of the table
   * @param name the name of the new table
   * @param table the new table
   */
  using SubTableListener =
      std::function<void(NetworkTable* parent, std::string_view name,
                         std::shared_ptr<NetworkTable> table)>;

  /**
   * Listen for sub-table creation. This calls the listener once for each newly
   * created sub-table. It immediately calls the listener for any existing
   * sub-tables.
   *
   * @param listener listener to add
   * @return Listener handle
   */
  NT_Listener AddSubTableListener(SubTableListener listener);

  /**
   * Remove a listener.
   *
   * @param listener listener handle
   */
  void RemoveListener(NT_Listener listener);
};

}  // namespace nt
