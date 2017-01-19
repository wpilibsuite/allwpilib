/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NETWORKTABLE_H_
#define NETWORKTABLE_H_

#include <functional>
#include <mutex>
#include <vector>

#include "tables/ITable.h"

/**
 * A network table that knows its subtable path.
 */
class NetworkTable : public ITable {
 private:
  struct private_init {};

  std::string m_path;
  std::mutex m_mutex;
  typedef std::pair<ITableListener*, unsigned int> Listener;
  std::vector<Listener> m_listeners;

  static std::vector<std::string> s_ip_addresses;
  static std::string s_persistent_filename;
  static bool s_client;
  static bool s_enable_ds;
  static bool s_running;
  static unsigned int s_port;

 public:
  NetworkTable(llvm::StringRef path, const private_init&);
  virtual ~NetworkTable();

  /**
   * The path separator for sub-tables and keys
   *
   */
  static const char PATH_SEPARATOR_CHAR;

  /**
   * @throws IOException
   */
  static void Initialize();
  static void Shutdown();

  /**
   * set that network tables should be a client
   * This must be called before initialize or GetTable
   */
  static void SetClientMode();

  /**
   * set that network tables should be a server
   * This must be called before initialize or GetTable
   */
  static void SetServerMode();

  /**
   * set the team the robot is configured for (this will set the mdns address
   * that network tables will connect to in client mode)
   * This must be called before initialize or GetTable
   * @param team the team number
   */
  static void SetTeam(int team);

  /**
   * @param address the adress that network tables will connect to in client
   * mode
   */
  static void SetIPAddress(llvm::StringRef address);

  /**
   * @param addresses the addresses that network tables will connect to in
   * client mode (in round robin order)
   */
  static void SetIPAddress(llvm::ArrayRef<std::string> addresses);

  /**
   * @param port the port number that network tables will connect to in client
   * mode or listen to in server mode
   */
  static void SetPort(unsigned int port);

  /**
   * @param enabled whether to enable the connection to the local DS to get
   * the robot IP address (defaults to enabled)
   */
  static void SetDSClientEnabled(bool enabled);

  /**
   * Sets the persistent filename.
   * @param filename the filename that the network tables server uses for
   * automatic loading and saving of persistent values
   */
  static void SetPersistentFilename(llvm::StringRef filename);

  /**
   * Sets the network identity.
   * This is provided in the connection info on the remote end.
   * @param name identity
   */
  static void SetNetworkIdentity(llvm::StringRef name);

  /**
   * Deletes ALL keys in ALL subtables.  Use with caution!
   */
  static void GlobalDeleteAll();

  /**
   * Flushes all updated values immediately to the network.
   * Note: This is rate-limited to protect the network from flooding.
   * This is primarily useful for synchronizing network updates with
   * user code.
   */
  static void Flush();

  /**
   * Set the periodic update rate.
   *
   * @param interval update interval in seconds (range 0.01 to 1.0)
   */
  static void SetUpdateRate(double interval);

  /**
   * Saves persistent keys to a file.  The server does this automatically.
   *
   * @param filename file name
   * @return Error (or nullptr).
   */
  static const char* SavePersistent(llvm::StringRef filename);

  /**
   * Loads persistent keys from a file.  The server does this automatically.
   *
   * @param filename file name
   * @param warn callback function called for warnings
   * @return Error (or nullptr).
   */
  static const char* LoadPersistent(
      llvm::StringRef filename,
      std::function<void(size_t line, const char* msg)> warn);

  /**
   * Gets the table with the specified key. If the table does not exist, a new
   * table will be created.<br>
   * This will automatically initialize network tables if it has not been
   * already.
   *
   * @param key
   *            the key name
   * @return the network table requested
   */
  static std::shared_ptr<NetworkTable> GetTable(llvm::StringRef key);

  void AddTableListener(ITableListener* listener) override;
  void AddTableListener(ITableListener* listener,
                        bool immediateNotify) override;
  void AddTableListenerEx(ITableListener* listener,
                          unsigned int flags) override;
  void AddTableListener(llvm::StringRef key, ITableListener* listener,
                        bool immediateNotify) override;
  void AddTableListenerEx(llvm::StringRef key, ITableListener* listener,
                          unsigned int flags) override;
  void AddSubTableListener(ITableListener* listener) override;
  void AddSubTableListener(ITableListener* listener, bool localNotify) override;
  void RemoveTableListener(ITableListener* listener) override;

  /**
   * Returns the table at the specified key. If there is no table at the
   * specified key, it will create a new table
   *
   * @param key
   *            the key name
   * @return the networktable to be returned
   */
  std::shared_ptr<ITable> GetSubTable(llvm::StringRef key) const override;

  /**
   * Determines whether the given key is in this table.
   *
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
  bool ContainsKey(llvm::StringRef key) const override;

  /**
   * Determines whether there exists a non-empty subtable for this key
   * in this table.
   *
   * @param key the key to search for
   * @return true if there is a subtable with the key which contains at least
   * one key/subtable of its own
   */
  bool ContainsSubTable(llvm::StringRef key) const override;

  /**
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  std::vector<std::string> GetKeys(int types = 0) const override;

  /**
   * @return subtables currently in the table
   */
  std::vector<std::string> GetSubTables() const override;

  /**
   * Makes a key's value persistent through program restarts.
   *
   * @param key the key to make persistent
   */
  void SetPersistent(llvm::StringRef key) override;

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  void ClearPersistent(llvm::StringRef key) override;

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  bool IsPersistent(llvm::StringRef key) const override;

  /**
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set (bitmask)
   */
  void SetFlags(llvm::StringRef key, unsigned int flags) override;

  /**
   * Clears flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to clear (bitmask)
   */
  void ClearFlags(llvm::StringRef key, unsigned int flags) override;

  /**
   * Returns the flags for the specified key.
   *
   * @param key the key name
   * @return the flags, or 0 if the key is not defined
   */
  unsigned int GetFlags(llvm::StringRef key) const override;

  /**
   * Deletes the specified key in this table.
   *
   * @param key the key name
   */
  void Delete(llvm::StringRef key) override;

  /**
   * Put a number in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutNumber(llvm::StringRef key, double value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultNumber(llvm::StringRef key,
                                double defaultValue) override;

  /**
   * Gets the number associated with the given name.
   *
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   * @deprecated This exception-raising method has been replaced by the
   * default-taking method.
   */
  WPI_DEPRECATED(
      "Raises an exception if key not found; "
      "use GetNumber(StringRef key, double defaultValue) instead")
  virtual double GetNumber(llvm::StringRef key) const override;

  /**
   * Gets the number associated with the given name.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  virtual double GetNumber(llvm::StringRef key,
                           double defaultValue) const override;

  /**
   * Put a string in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutString(llvm::StringRef key, llvm::StringRef value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultString(llvm::StringRef key,
                                llvm::StringRef defaultValue) override;

  /**
   * Gets the string associated with the given name.
   *
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   * @deprecated This exception-raising method has been replaced by the
   * default-taking method.
   */
  WPI_DEPRECATED(
      "Raises an exception if key not found; "
      "use GetString(StringRef key, StringRef defaultValue) instead")
  virtual std::string GetString(llvm::StringRef key) const override;

  /**
   * Gets the string associated with the given name. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  virtual std::string GetString(llvm::StringRef key,
                                llvm::StringRef defaultValue) const override;

  /**
   * Put a boolean in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutBoolean(llvm::StringRef key, bool value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultBoolean(llvm::StringRef key,
                                 bool defaultValue) override;

  /**
   * Gets the boolean associated with the given name.
   *
   * @param key the key to look up
   * @return the value associated with the given key
   * @throws TableKeyNotDefinedException if there is no value associated with
   * the given key
   * @deprecated This exception-raising method has been replaced by the
   * default-taking method.
   */
  WPI_DEPRECATED(
      "Raises an exception if key not found; "
      "use GetBoolean(StringRef key, bool defaultValue) instead")
  virtual bool GetBoolean(llvm::StringRef key) const override;

  /**
   * Gets the boolean associated with the given name. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  virtual bool GetBoolean(llvm::StringRef key,
                          bool defaultValue) const override;

  /**
   * Put a boolean array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   *
   * @note The array must be of int's rather than of bool's because
   *       std::vector<bool> is special-cased in C++.  0 is false, any
   *       non-zero value is true.
   */
  virtual bool PutBooleanArray(llvm::StringRef key,
                               llvm::ArrayRef<int> value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultBooleanArray(
      llvm::StringRef key, llvm::ArrayRef<int> defaultValue) override;

  /**
   * Returns the boolean array the key maps to. If the key does not exist or is
   * of different type, it will return the default value.
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
  virtual std::vector<int> GetBooleanArray(
      llvm::StringRef key, llvm::ArrayRef<int> defaultValue) const override;

  /**
   * Put a number array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutNumberArray(llvm::StringRef key,
                              llvm::ArrayRef<double> value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultNumberArray(
      llvm::StringRef key, llvm::ArrayRef<double> defaultValue) override;

  /**
   * Returns the number array the key maps to. If the key does not exist or is
   * of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  virtual std::vector<double> GetNumberArray(
      llvm::StringRef key, llvm::ArrayRef<double> defaultValue) const override;

  /**
   * Put a string array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutStringArray(llvm::StringRef key,
                              llvm::ArrayRef<std::string> value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultStringArray(
      llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue) override;

  /**
   * Returns the string array the key maps to. If the key does not exist or is
   * of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the array.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  virtual std::vector<std::string> GetStringArray(
      llvm::StringRef key,
      llvm::ArrayRef<std::string> defaultValue) const override;

  /**
   * Put a raw value (byte array) in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutRaw(llvm::StringRef key, llvm::StringRef value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultRaw(llvm::StringRef key,
                             llvm::StringRef defaultValue) override;

  /**
   * Returns the raw value (byte array) the key maps to. If the key does not
   * exist or is of different type, it will return the default value.
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the raw contents.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  virtual std::string GetRaw(llvm::StringRef key,
                             llvm::StringRef defaultValue) const override;

  /**
   * Put a value in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  bool PutValue(llvm::StringRef key, std::shared_ptr<nt::Value> value) override;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultValue(
      llvm::StringRef key, std::shared_ptr<nt::Value> defaultValue) override;

  /**
   * Gets the value associated with a key as an object
   *
   * @param key the key of the value to look up
   * @return the value associated with the given key, or nullptr if the key
   * does not exist
   */
  std::shared_ptr<nt::Value> GetValue(llvm::StringRef key) const override;
};

#endif  // NETWORKTABLE_H_
