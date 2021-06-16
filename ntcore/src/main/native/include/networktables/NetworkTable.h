// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_NETWORKTABLE_H_
#define NTCORE_NETWORKTABLES_NETWORKTABLE_H_

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/StringMap.h>
#include <wpi/mutex.h>
#include <wpi/span.h>

#include "networktables/NetworkTableEntry.h"
#include "networktables/TableEntryListener.h"
#include "networktables/TableListener.h"
#include "ntcore_c.h"

namespace nt {

class NetworkTableInstance;

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
  std::vector<NT_EntryListener> m_listeners;

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
  virtual ~NetworkTable();

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
   * Listen to keys only within this table.
   *
   * @param listener    listener to add
   * @param flags       EntryListenerFlags bitmask
   * @return Listener handle
   */
  NT_EntryListener AddEntryListener(TableEntryListener listener,
                                    unsigned int flags) const;

  /**
   * Listen to a single key.
   *
   * @param key         the key name
   * @param listener    listener to add
   * @param flags       EntryListenerFlags bitmask
   * @return Listener handle
   */
  NT_EntryListener AddEntryListener(std::string_view key,
                                    TableEntryListener listener,
                                    unsigned int flags) const;

  /**
   * Remove an entry listener.
   *
   * @param listener    listener handle
   */
  void RemoveEntryListener(NT_EntryListener listener) const;

  /**
   * Listen for sub-table creation.
   * This calls the listener once for each newly created sub-table.
   * It immediately calls the listener for any existing sub-tables.
   *
   * @param listener        listener to add
   * @param localNotify     notify local changes as well as remote
   * @return Listener handle
   */
  NT_EntryListener AddSubTableListener(TableListener listener,
                                       bool localNotify = false);

  /**
   * Remove a sub-table listener.
   *
   * @param listener    listener handle
   */
  void RemoveTableListener(NT_EntryListener listener);

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
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set (bitmask)
   */
  void SetFlags(std::string_view key, unsigned int flags);

  /**
   * Clears flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to clear (bitmask)
   */
  void ClearFlags(std::string_view key, unsigned int flags);

  /**
   * Returns the flags for the specified key.
   *
   * @param key the key name
   * @return the flags, or 0 if the key is not defined
   */
  unsigned int GetFlags(std::string_view key) const;

  /**
   * Deletes the specified key in this table.
   *
   * @param key the key name
   */
  void Delete(std::string_view key);

  /**
   * Put a number in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutNumber(std::string_view key, double value);

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
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
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
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
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
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
  bool PutBooleanArray(std::string_view key, wpi::span<const int> value);

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return False if the table key exists with a different type
   */
  bool SetDefaultBooleanArray(std::string_view key,
                              wpi::span<const int> defaultValue);

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
                                   wpi::span<const int> defaultValue) const;

  /**
   * Put a number array in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutNumberArray(std::string_view key, wpi::span<const double> value);

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  bool SetDefaultNumberArray(std::string_view key,
                             wpi::span<const double> defaultValue);

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
      std::string_view key, wpi::span<const double> defaultValue) const;

  /**
   * Put a string array in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutStringArray(std::string_view key, wpi::span<const std::string> value);

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  bool SetDefaultStringArray(std::string_view key,
                             wpi::span<const std::string> defaultValue);

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
      std::string_view key, wpi::span<const std::string> defaultValue) const;

  /**
   * Put a raw value (byte array) in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutRaw(std::string_view key, std::string_view value);

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return False if the table key exists with a different type
   */
  bool SetDefaultRaw(std::string_view key, std::string_view defaultValue);

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
  std::string GetRaw(std::string_view key, std::string_view defaultValue) const;

  /**
   * Put a value in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutValue(std::string_view key, std::shared_ptr<Value> value);

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return False if the table key exists with a different type
   */
  bool SetDefaultValue(std::string_view key,
                       std::shared_ptr<Value> defaultValue);

  /**
   * Gets the value associated with a key as an object
   *
   * @param key the key of the value to look up
   * @return the value associated with the given key, or nullptr if the key
   * does not exist
   */
  std::shared_ptr<Value> GetValue(std::string_view key) const;

  /**
   * Gets the full path of this table.  Does not include the trailing "/".
   *
   * @return The path (e.g "", "/foo").
   */
  std::string_view GetPath() const;

  /**
   * Save table values to a file.  The file format used is identical to
   * that used for SavePersistent.
   *
   * @param filename  filename
   * @return error string, or nullptr if successful
   */
  const char* SaveEntries(std::string_view filename) const;

  /**
   * Load table values from a file.  The file format used is identical to
   * that used for SavePersistent / LoadPersistent.
   *
   * @param filename  filename
   * @param warn      callback function for warnings
   * @return error string, or nullptr if successful
   */
  const char* LoadEntries(
      std::string_view filename,
      std::function<void(size_t line, const char* msg)> warn);
};

}  // namespace nt

#endif  // NTCORE_NETWORKTABLES_NETWORKTABLE_H_
