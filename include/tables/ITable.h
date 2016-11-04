/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef ITABLE_H_
#define ITABLE_H_

#include <memory>

#include "llvm/StringRef.h"
#include "nt_Value.h"
#include "support/deprecated.h"

class ITableListener;

/**
 * A table whose values can be read and written to
 */
class ITable {
 public:
  /**
   * Determines whether the given key is in this table.
   *
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
  virtual bool ContainsKey(llvm::StringRef key) const = 0;

  /**
   * Determines whether there exists a non-empty subtable for this key
   * in this table.
   *
   * @param key the key to search for
   * @return true if there is a subtable with the key which contains at least
   * one key/subtable of its own
   */
  virtual bool ContainsSubTable(llvm::StringRef key) const = 0;

  /**
   * Gets the subtable in this table for the given name.
   *
   * @param key the name of the table relative to this one
   * @return a sub table relative to this one
   */
  virtual std::shared_ptr<ITable> GetSubTable(llvm::StringRef key) const = 0;

  /**
   * @param types bitmask of types; 0 is treated as a "don't care".
   * @return keys currently in the table
   */
  virtual std::vector<std::string> GetKeys(int types = 0) const = 0;

  /**
   * @return subtables currently in the table
   */
  virtual std::vector<std::string> GetSubTables() const = 0;

  /**
   * Makes a key's value persistent through program restarts.
   *
   * @param key the key to make persistent
   */
  virtual void SetPersistent(llvm::StringRef key) = 0;

  /**
   * Stop making a key's value persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  virtual void ClearPersistent(llvm::StringRef key) = 0;

  /**
   * Returns whether the value is persistent through program restarts.
   * The key cannot be null.
   *
   * @param key the key name
   */
  virtual bool IsPersistent(llvm::StringRef key) const = 0;

  /**
   * Sets flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to set (bitmask)
   */
  virtual void SetFlags(llvm::StringRef key, unsigned int flags) = 0;

  /**
   * Clears flags on the specified key in this table. The key can
   * not be null.
   *
   * @param key the key name
   * @param flags the flags to clear (bitmask)
   */
  virtual void ClearFlags(llvm::StringRef key, unsigned int flags) = 0;

  /**
   * Returns the flags for the specified key.
   *
   * @param key the key name
   * @return the flags, or 0 if the key is not defined
   */
  virtual unsigned int GetFlags(llvm::StringRef key) const = 0;

  /**
   * Deletes the specified key in this table.
   *
   * @param key the key name
   */
  virtual void Delete(llvm::StringRef key) = 0;

  /**
   * Gets the value associated with a key as an object
   *
   * @param key the key of the value to look up
   * @return the value associated with the given key, or nullptr if the key
   * does not exist
   */
  virtual std::shared_ptr<nt::Value> GetValue(llvm::StringRef key) const = 0;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultValue(llvm::StringRef key,
                               std::shared_ptr<nt::Value> defaultValue) = 0;

  /**
   * Put a value in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutValue(llvm::StringRef key,
                        std::shared_ptr<nt::Value> value) = 0;

  /**
   * Put a number in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutNumber(llvm::StringRef key, double value) = 0;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultNumber(llvm::StringRef key, double defaultValue) = 0;

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
  virtual double GetNumber(llvm::StringRef key) const = 0;

  /**
   * Gets the number associated with the given name.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  virtual double GetNumber(llvm::StringRef key, double defaultValue) const = 0;

  /**
   * Put a string in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutString(llvm::StringRef key, llvm::StringRef value) = 0;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultString(llvm::StringRef key,
                                llvm::StringRef defaultValue) = 0;

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
  virtual std::string GetString(llvm::StringRef key) const = 0;

  /**
   * Gets the string associated with the given name. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   *
   * @note This makes a copy of the string.  If the overhead of this is a
   *       concern, use GetValue() instead.
   */
  virtual std::string GetString(llvm::StringRef key,
                                llvm::StringRef defaultValue) const = 0;

  /**
   * Put a boolean in the table
   *
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutBoolean(llvm::StringRef key, bool value) = 0;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultBoolean(llvm::StringRef key, bool defaultValue) = 0;

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
  virtual bool GetBoolean(llvm::StringRef key) const = 0;

  /**
   * Gets the boolean associated with the given name. If the key does not
   * exist or is of different type, it will return the default value.
   *
   * @param key the key to look up
   * @param defaultValue the value to be returned if no value is found
   * @return the value associated with the given key or the given default value
   * if there is no value associated with the key
   */
  virtual bool GetBoolean(llvm::StringRef key, bool defaultValue) const = 0;

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
                               llvm::ArrayRef<int> value) = 0;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultBooleanArray(llvm::StringRef key,
                                      llvm::ArrayRef<int> defaultValue) = 0;

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
      llvm::StringRef key, llvm::ArrayRef<int> defaultValue) const = 0;

  /**
   * Put a number array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutNumberArray(llvm::StringRef key,
                              llvm::ArrayRef<double> value) = 0;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultNumberArray(llvm::StringRef key,
                                     llvm::ArrayRef<double> defaultValue) = 0;

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
      llvm::StringRef key, llvm::ArrayRef<double> defaultValue) const = 0;

  /**
   * Put a string array in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutStringArray(llvm::StringRef key,
                              llvm::ArrayRef<std::string> value) = 0;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultStringArray(
      llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue) = 0;

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
      llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue) const = 0;

  /**
   * Put a raw value (byte array) in the table
   * @param key the key to be assigned to
   * @param value the value that will be assigned
   * @return False if the table key already exists with a different type
   */
  virtual bool PutRaw(llvm::StringRef key, llvm::StringRef value) = 0;

  /**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
  virtual bool SetDefaultRaw(llvm::StringRef key,
                             llvm::StringRef defaultValue) = 0;

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
                             llvm::StringRef defaultValue) const = 0;

  /**
   * Add a listener for changes to the table
   *
   * @param listener the listener to add
   */
  virtual void AddTableListener(ITableListener* listener) = 0;

  /**
   * Add a listener for changes to the table
   *
   * @param listener the listener to add
   * @param immediateNotify if true then this listener will be notified of all
   * current entries (marked as new)
   */
  virtual void AddTableListener(ITableListener* listener,
                                bool immediateNotify) = 0;

  /**
   * Add a listener for changes to the table
   *
   * @param listener the listener to add
   * @param immediateNotify if true then this listener will be notified of all
   * current entries (marked as new)
   * @param flags bitmask of NT_NotifyKind specifying desired notifications
   */
  virtual void AddTableListenerEx(ITableListener* listener,
                                  unsigned int flags) = 0;

  /**
   * Add a listener for changes to a specific key the table
   *
   * @param key the key to listen for
   * @param listener the listener to add
   * @param immediateNotify if true then this listener will be notified of all
   * current entries (marked as new)
   */
  virtual void AddTableListener(llvm::StringRef key, ITableListener* listener,
                                bool immediateNotify) = 0;

  /**
   * Add a listener for changes to a specific key the table
   *
   * @param key the key to listen for
   * @param listener the listener to add
   * @param immediateNotify if true then this listener will be notified of all
   * current entries (marked as new)
   * @param flags bitmask of NT_NotifyKind specifying desired notifications
   */
  virtual void AddTableListenerEx(llvm::StringRef key, ITableListener* listener,
                                  unsigned int flags) = 0;

  /**
   * This will immediately notify the listener of all current sub tables
   * @param listener the listener to add
   */
  virtual void AddSubTableListener(ITableListener* listener) = 0;

  /**
   * This will immediately notify the listener of all current sub tables
   * @param listener the listener to add
   * @param localNotify if true then this listener will be notified of all
   * local changes in addition to all remote changes
   */
  virtual void AddSubTableListener(ITableListener* listener,
                                   bool localNotify) = 0;

  /**
   * Remove a listener from receiving table events
   *
   * @param listener the listener to be removed
   */
  virtual void RemoveTableListener(ITableListener* listener) = 0;
};

#endif  // ITABLE_H_
