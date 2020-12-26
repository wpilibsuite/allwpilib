// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef NTCORE_NETWORKTABLES_NETWORKTABLE_H_
#define NTCORE_NETWORKTABLES_NETWORKTABLE_H_

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/StringMap.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "networktables/NetworkTableEntry.h"
#include "networktables/TableEntryListener.h"
#include "networktables/TableListener.h"
#include "ntcore_c.h"
#include "tables/ITable.h"

namespace nt {

using wpi::ArrayRef;
using wpi::StringRef;
using wpi::Twine;

class NetworkTableInstance;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

/**
 * @defgroup ntcore_cpp_api ntcore C++ object-oriented API
 *
 * Recommended interface for C++, identical to Java API.
 */

/**
 * A network table that knows its subtable path.
 * @ingroup ntcore_cpp_api
 */
class NetworkTable final : public ITable {
 private:
  NT_Inst m_inst;
  std::string m_path;
  mutable wpi::mutex m_mutex;
  mutable wpi::StringMap<NT_Entry> m_entries;
  typedef std::pair<ITableListener*, NT_EntryListener> Listener;
  std::vector<Listener> m_listeners;
  std::vector<NT_EntryListener> m_lambdaListeners;

  static std::vector<std::string> s_ip_addresses;
  static std::string s_persistent_filename;
  static bool s_client;
  static bool s_enable_ds;
  static bool s_running;
  static unsigned int s_port;

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
  static StringRef BasenameKey(StringRef key);

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
  static std::string NormalizeKey(const Twine& key,
                                  bool withLeadingSlash = true);

  static StringRef NormalizeKey(const Twine& key,
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
  static std::vector<std::string> GetHierarchy(const Twine& key);

  /**
   * Constructor.  Use NetworkTableInstance::GetTable() or GetSubTable()
   * instead.
   */
  NetworkTable(NT_Inst inst, const Twine& path, const private_init&);
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
  static const char PATH_SEPARATOR_CHAR;

  /**
   * Initializes network tables
   */
  WPI_DEPRECATED(
      "use NetworkTableInstance::StartServer() or "
      "NetworkTableInstance::StartClient() instead")
  static void Initialize();

  /**
   * Shuts down network tables
   */
  WPI_DEPRECATED(
      "use NetworkTableInstance::StopServer() or "
      "NetworkTableInstance::StopClient() instead")
  static void Shutdown();

  /**
   * set that network tables should be a client
   * This must be called before initialize or GetTable
   */
  WPI_DEPRECATED("use NetworkTableInstance::StartClient() instead")
  static void SetClientMode();

  /**
   * set that network tables should be a server
   * This must be called before initialize or GetTable
   */
  WPI_DEPRECATED("use NetworkTableInstance::StartServer() instead")
  static void SetServerMode();

  /**
   * set the team the robot is configured for (this will set the mdns address
   * that network tables will connect to in client mode)
   * This must be called before initialize or GetTable
   *
   * @param team the team number
   */
  WPI_DEPRECATED(
      "use NetworkTableInstance::SetServerTeam() or "
      "NetworkTableInstance::StartClientTeam() instead")
  static void SetTeam(int team);

  /**
   * @param address the address that network tables will connect to in client
   * mode
   */
  WPI_DEPRECATED(
      "use NetworkTableInstance::SetServer() or "
      "NetworkTableInstance::StartClient() instead")
  static void SetIPAddress(StringRef address);

  /**
   * @param addresses the addresses that network tables will connect to in
   * client mode (in round robin order)
   */
  WPI_DEPRECATED(
      "use NetworkTableInstance::SetServer() or "
      "NetworkTableInstance::StartClient() instead")
  static void SetIPAddress(ArrayRef<std::string> addresses);

  /**
   * Set the port number that network tables will connect to in client
   * mode or listen to in server mode.
   *
   * @param port the port number
   */
  WPI_DEPRECATED(
      "use the appropriate parameters to NetworkTableInstance::SetServer(), "
      "NetworkTableInstance::StartClient(), "
      "NetworkTableInstance::StartServer(), and "
      "NetworkTableInstance::StartDSClient() instead")
  static void SetPort(unsigned int port);

  /**
   * Enable requesting the server address from the Driver Station.
   *
   * @param enabled whether to enable the connection to the local DS
   */
  WPI_DEPRECATED(
      "use NetworkTableInstance::StartDSClient() and "
      "NetworkTableInstance::StopDSClient() instead")
  static void SetDSClientEnabled(bool enabled);

  /**
   * Sets the persistent filename.
   *
   * @param filename the filename that the network tables server uses for
   * automatic loading and saving of persistent values
   */
  WPI_DEPRECATED(
      "use the appropriate parameter to NetworkTableInstance::StartServer() "
      "instead")
  static void SetPersistentFilename(StringRef filename);

  /**
   * Sets the network identity.
   * This is provided in the connection info on the remote end.
   *
   * @param name identity
   */
  WPI_DEPRECATED("use NetworkTableInstance::SetNetworkIdentity() instead")
  static void SetNetworkIdentity(StringRef name);

  /**
   * Deletes ALL keys in ALL subtables.  Use with caution!
   */
  WPI_DEPRECATED("use NetworkTableInstance::DeleteAllEntries() instead")
  static void GlobalDeleteAll();

  /**
   * Flushes all updated values immediately to the network.
   * Note: This is rate-limited to protect the network from flooding.
   * This is primarily useful for synchronizing network updates with
   * user code.
   */
  WPI_DEPRECATED("use NetworkTableInstance::Flush() instead")
  static void Flush();

  /**
   * Set the periodic update rate.
   * Sets how frequently updates are sent to other nodes over the network.
   *
   * @param interval update interval in seconds (range 0.01 to 1.0)
   */
  WPI_DEPRECATED("use NetworkTableInstance::SetUpdateRate() instead")
  static void SetUpdateRate(double interval);

  /**
   * Saves persistent keys to a file.  The server does this automatically.
   *
   * @param filename file name
   * @return Error (or nullptr).
   */
  WPI_DEPRECATED("use NetworkTableInstance::SavePersistent() instead")
  static const char* SavePersistent(StringRef filename);

  /**
   * Loads persistent keys from a file.  The server does this automatically.
   *
   * @param filename file name
   * @param warn callback function called for warnings
   * @return Error (or nullptr).
   */
  WPI_DEPRECATED("use NetworkTableInstance::LoadPersistent() instead")
  static const char* LoadPersistent(
      StringRef filename,
      std::function<void(size_t line, const char* msg)> warn);

  /**
   * Gets the table with the specified key. If the table does not exist, a new
   * table will be created.<br>
   * This will automatically initialize network tables if it has not been
   * already.
   *
   * @param key  the key name
   * @return the network table requested
   */
  WPI_DEPRECATED(
      "use NetworkTableInstance::GetTable() or "
      "NetworkTableInstance::GetEntry() instead")
  static std::shared_ptr<NetworkTable> GetTable(StringRef key);

  /**
   * Gets the entry for a subkey.
   *
   * @param key the key name
   * @return Network table entry.
   */
  NetworkTableEntry GetEntry(const Twine& key) const;

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
  NT_EntryListener AddEntryListener(const Twine& key,
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

  WPI_DEPRECATED(
      "use AddEntryListener() instead with flags value of NT_NOTIFY_NEW | "
      "NT_NOTIFY_UPDATE")
  void AddTableListener(ITableListener* listener) override;

  WPI_DEPRECATED(
      "use AddEntryListener() instead with flags value of NT_NOTIFY_NEW | "
      "NT_NOTIFY_UPDATE | NT_NOTIFY_IMMEDIATE")
  void AddTableListener(ITableListener* listener,
                        bool immediateNotify) override;

  WPI_DEPRECATED("use AddEntryListener() instead")
  void AddTableListenerEx(ITableListener* listener,
                          unsigned int flags) override;

  WPI_DEPRECATED("use AddEntryListener() instead")
  void AddTableListener(StringRef key, ITableListener* listener,
                        bool immediateNotify) override;

  WPI_DEPRECATED("use AddEntryListener() instead")
  void AddTableListenerEx(StringRef key, ITableListener* listener,
                          unsigned int flags) override;

  WPI_DEPRECATED("use AddSubTableListener(TableListener, bool) instead")
  void AddSubTableListener(ITableListener* listener) override;

  WPI_DEPRECATED("use AddSubTableListener(TableListener, bool) instead")
  void AddSubTableListener(ITableListener* listener, bool localNotify) override;

  WPI_DEPRECATED("use RemoveTableListener(NT_EntryListener) instead")
  void RemoveTableListener(ITableListener* listener) override;

  /**
   * Returns the table at the specified key. If there is no table at the
   * specified key, it will create a new table
   *
   * @param key the key name
   * @return the networktable to be returned
   */
  std::shared_ptr<NetworkTable> GetSubTable(const Twine& key) const override;

  /**
   * Determines whether the given key is in this table.
   *
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
  bool ContainsKey(const Twine& key) const override;

  /**
   * Determines whether there exists a non-empty subtable for this key
   * in this table.
   *
   * @param key the key to search for
   * @return true if there is a subtable with the key which contains at least
   * one key/subtable of its own
   */
  bool ContainsSubTable(const Twine& key) const override;

  /**
   * Gets all keys in the table (not including sub-tables).
   *
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  std::vector<std::string> GetKeys(int types = 0) const override;

  /**
   * Gets the names of all subtables in the table.
   *
   * @return subtables currently in the table
   */
  std::vector<std::string> GetSubTables() const override;

  /**
   * Makes a key's value persistent through program restarts.
   *
   * @param key the key to make persistent
   */
  void SetPersistent(StringRef key) override;

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  void ClearPersistent(StringRef key) override;

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  bool IsPersistent(StringRef key) const override;

  /**
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set (bitmask)
   */
  void SetFlags(StringRef key, unsigned int flags) override;

  /**
   * Clears flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to clear (bitmask)
   */
  void ClearFlags(StringRef key, unsigned int flags) override;

  /**
   * Returns the flags for the specified key.
   *
   * @param key the key name
   * @return the flags, or 0 if the key is not defined
   */
  unsigned int GetFlags(StringRef key) const override;

  /**
   * Deletes the specified key in this table.
   *
   * @param key the key name
   */
  void Delete(const Twine& key) override;

  /**
   * Put a number in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutNumber(StringRef key, double value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  bool SetDefaultNumber(StringRef key, double defaultValue) override;

  /**
   * Gets the number associated with the given name.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  double GetNumber(StringRef key, double defaultValue) const override;

  /**
   * Put a string in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutString(StringRef key, StringRef value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  bool SetDefaultString(StringRef key, StringRef defaultValue) override;

  /**
   * Gets the string associated with the given name. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  std::string GetString(StringRef key, StringRef defaultValue) const override;

  /**
   * Put a boolean in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutBoolean(StringRef key, bool value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  bool SetDefaultBoolean(StringRef key, bool defaultValue) override;

  /**
   * Gets the boolean associated with the given name. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  bool GetBoolean(StringRef key, bool defaultValue) const override;

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
  bool PutBooleanArray(StringRef key, ArrayRef<int> value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return False if the table key exists with a different type
   */
  bool SetDefaultBooleanArray(StringRef key,
                              ArrayRef<int> defaultValue) override;

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
  std::vector<int> GetBooleanArray(StringRef key,
                                   ArrayRef<int> defaultValue) const override;

  /**
   * Put a number array in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutNumberArray(StringRef key, ArrayRef<double> value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  bool SetDefaultNumberArray(StringRef key,
                             ArrayRef<double> defaultValue) override;

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
      StringRef key, ArrayRef<double> defaultValue) const override;

  /**
   * Put a string array in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutStringArray(StringRef key, ArrayRef<std::string> value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  bool SetDefaultStringArray(StringRef key,
                             ArrayRef<std::string> defaultValue) override;

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
      StringRef key, ArrayRef<std::string> defaultValue) const override;

  /**
   * Put a raw value (byte array) in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutRaw(StringRef key, StringRef value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return False if the table key exists with a different type
   */
  bool SetDefaultRaw(StringRef key, StringRef defaultValue) override;

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
  std::string GetRaw(StringRef key, StringRef defaultValue) const override;

  /**
   * Put a value in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutValue(const Twine& key, std::shared_ptr<Value> value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   *
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @return False if the table key exists with a different type
   */
  bool SetDefaultValue(const Twine& key,
                       std::shared_ptr<Value> defaultValue) override;

  /**
   * Gets the value associated with a key as an object
   *
   * @param key the key of the value to look up
   * @return the value associated with the given key, or nullptr if the key
   * does not exist
   */
  std::shared_ptr<Value> GetValue(const Twine& key) const override;

  /**
   * Gets the full path of this table.  Does not include the trailing "/".
   *
   * @return The path (e.g "", "/foo").
   */
  StringRef GetPath() const override;

  /**
   * Save table values to a file.  The file format used is identical to
   * that used for SavePersistent.
   *
   * @param filename  filename
   * @return error string, or nullptr if successful
   */
  const char* SaveEntries(const Twine& filename) const;

  /**
   * Load table values from a file.  The file format used is identical to
   * that used for SavePersistent / LoadPersistent.
   *
   * @param filename  filename
   * @param warn      callback function for warnings
   * @return error string, or nullptr if successful
   */
  const char* LoadEntries(
      const Twine& filename,
      std::function<void(size_t line, const char* msg)> warn);
};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#elif _WIN32
#pragma warning(pop)
#endif

}  // namespace nt

// For backwards compatibility
#ifndef NAMESPACED_NT
using nt::NetworkTable;  // NOLINT
#endif

#endif  // NTCORE_NETWORKTABLES_NETWORKTABLE_H_
