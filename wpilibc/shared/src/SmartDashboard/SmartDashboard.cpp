/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SmartDashboard.h"

#include "HLUsageReporting.h"
#include "SmartDashboard/NamedSendable.h"
#include "WPIErrors.h"
#include "networktables/NetworkTable.h"

using namespace frc;

std::shared_ptr<ITable> SmartDashboard::m_table;
std::map<std::shared_ptr<ITable>, Sendable*> SmartDashboard::m_tablesToData;

void SmartDashboard::init() {
  m_table = NetworkTable::GetTable("SmartDashboard");

  HLUsageReporting::ReportSmartDashboard();
}

/**
   * Determines whether the given key is in this table.
   *
   * @param key the key to search for
   * @return true if the table as a value assigned to the given key
   */
bool SmartDashboard::ContainsKey(llvm::StringRef key) {
  return m_table->ContainsKey(key);
}

/**
 * @param types bitmask of types; 0 is treated as a "don't care".
 * @return keys currently in the table
 */
std::vector<std::string> SmartDashboard::GetKeys(int types) {
  return m_table->GetKeys(types);
}

/**
 * Makes a key's value persistent through program restarts.
 *
 * @param key the key to make persistent
 */
void SmartDashboard::SetPersistent(llvm::StringRef key) {
  m_table->SetPersistent(key);
}

/**
 * Stop making a key's value persistent through program restarts.
 * The key cannot be null.
 *
 * @param key the key name
 */
void SmartDashboard::ClearPersistent(llvm::StringRef key) {
  m_table->ClearPersistent(key);
}

/**
 * Returns whether the value is persistent through program restarts.
 * The key cannot be null.
 *
 * @param key the key name
 */
bool SmartDashboard::IsPersistent(llvm::StringRef key) {
  return m_table->IsPersistent(key);
}

/**
 * Sets flags on the specified key in this table. The key can
 * not be null.
 *
 * @param key the key name
 * @param flags the flags to set (bitmask)
 */
void SmartDashboard::SetFlags(llvm::StringRef key, unsigned int flags) {
  m_table->SetFlags(key, flags);
}

/**
 * Clears flags on the specified key in this table. The key can
 * not be null.
 *
 * @param key the key name
 * @param flags the flags to clear (bitmask)
 */
void SmartDashboard::ClearFlags(llvm::StringRef key, unsigned int flags) {
  m_table->ClearFlags(key, flags);
}

/**
 * Returns the flags for the specified key.
 *
 * @param key the key name
 * @return the flags, or 0 if the key is not defined
 */
unsigned int SmartDashboard::GetFlags(llvm::StringRef key) {
  return m_table->GetFlags(key);
}

/**
 * Deletes the specified key in this table.
 *
 * @param key the key name
 */
void SmartDashboard::Delete(llvm::StringRef key) { m_table->Delete(key); }

/**
 * Maps the specified key to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param keyName the key
 * @param value   the value
 */
void SmartDashboard::PutData(llvm::StringRef key, Sendable* data) {
  if (data == nullptr) {
    wpi_setGlobalWPIErrorWithContext(NullParameter, "value");
    return;
  }
  std::shared_ptr<ITable> dataTable(m_table->GetSubTable(key));
  dataTable->PutString("~TYPE~", data->GetSmartDashboardType());
  data->InitTable(dataTable);
  m_tablesToData[dataTable] = data;
}

/**
 * Maps the specified key (where the key is the name of the
 * {@link SmartDashboardNamedData} to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param value the value
 */
void SmartDashboard::PutData(NamedSendable* value) {
  if (value == nullptr) {
    wpi_setGlobalWPIErrorWithContext(NullParameter, "value");
    return;
  }
  PutData(value->GetName(), value);
}

/**
 * Returns the value at the specified key.
 *
 * @param keyName the key
 * @return the value
 */
Sendable* SmartDashboard::GetData(llvm::StringRef key) {
  std::shared_ptr<ITable> subtable(m_table->GetSubTable(key));
  Sendable* data = m_tablesToData[subtable];
  if (data == nullptr) {
    wpi_setGlobalWPIErrorWithContext(SmartDashboardMissingKey, key);
    return nullptr;
  }
  return data;
}

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
bool SmartDashboard::PutValue(llvm::StringRef keyName,
                              std::shared_ptr<nt::Value> value) {
  return m_table->PutValue(keyName, value);
}

/**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
bool SmartDashboard::SetDefaultValue(llvm::StringRef key,
                                     std::shared_ptr<nt::Value> defaultValue) {
  return m_table->SetDefaultValue(key, defaultValue);
}

/**
 * Retrieves the complex value (such as an array) in this table into the complex
 * data object.
 *
 * @param keyName the key
 * @param value   the object to retrieve the value into
 */
std::shared_ptr<nt::Value> SmartDashboard::GetValue(llvm::StringRef keyName) {
  return m_table->GetValue(keyName);
}

/**
 * Maps the specified key to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param keyName the key
 * @param value   the value
 * @return        False if the table key already exists with a different type
 */
bool SmartDashboard::PutBoolean(llvm::StringRef keyName, bool value) {
  return m_table->PutBoolean(keyName, value);
}

/**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
bool SmartDashboard::SetDefaultBoolean(llvm::StringRef key, bool defaultValue) {
  return m_table->SetDefaultBoolean(key, defaultValue);
}

/**
 * Returns the value at the specified key.
 *
 * If the key is not found, returns the default value.
 *
 * @param keyName the key
 * @return the value
 */
bool SmartDashboard::GetBoolean(llvm::StringRef keyName, bool defaultValue) {
  return m_table->GetBoolean(keyName, defaultValue);
}

/**
 * Maps the specified key to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param keyName the key
 * @param value   the value
 * @return        False if the table key already exists with a different type
 */
bool SmartDashboard::PutNumber(llvm::StringRef keyName, double value) {
  return m_table->PutNumber(keyName, value);
}

/**
   * Gets the current value in the table, setting it if it does not exist.
   * @param key the key
   * @param defaultValue the default value to set if key doesn't exist.
   * @returns False if the table key exists with a different type
   */
bool SmartDashboard::SetDefaultNumber(llvm::StringRef key,
                                      double defaultValue) {
  return m_table->SetDefaultNumber(key, defaultValue);
}

/**
 * Returns the value at the specified key.
 *
 * If the key is not found, returns the default value.
 *
 * @param keyName the key
 * @return the value
 */
double SmartDashboard::GetNumber(llvm::StringRef keyName, double defaultValue) {
  return m_table->GetNumber(keyName, defaultValue);
}

/**
 * Maps the specified key to the specified value in this table.
 *
 * The value can be retrieved by calling the get method with a key that is equal
 * to the original key.
 *
 * @param keyName the key
 * @param value   the value
 * @return        False if the table key already exists with a different type
 */
bool SmartDashboard::PutString(llvm::StringRef keyName, llvm::StringRef value) {
  return m_table->PutString(keyName, value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 * @param key the key
 * @param defaultValue the default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultString(llvm::StringRef key,
                                      llvm::StringRef defaultValue) {
  return m_table->SetDefaultString(key, defaultValue);
}

/**
 * Returns the value at the specified key.
 *
 * If the key is not found, returns the default value.
 *
 * @param keyName the key
 * @return the value
 */
std::string SmartDashboard::GetString(llvm::StringRef keyName,
                                      llvm::StringRef defaultValue) {
  return m_table->GetString(keyName, defaultValue);
}

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
bool SmartDashboard::PutBooleanArray(llvm::StringRef key,
                                     llvm::ArrayRef<int> value) {
  return m_table->PutBooleanArray(key, value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 * @param key the key
 * @param defaultValue the default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultBooleanArray(llvm::StringRef key,
                                            llvm::ArrayRef<int> defaultValue) {
  return m_table->SetDefaultBooleanArray(key, defaultValue);
}

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
std::vector<int> SmartDashboard::GetBooleanArray(
    llvm::StringRef key, llvm::ArrayRef<int> defaultValue) {
  return m_table->GetBooleanArray(key, defaultValue);
}

/**
 * Put a number array in the table
 * @param key the key to be assigned to
 * @param value the value that will be assigned
 * @return False if the table key already exists with a different type
 */
bool SmartDashboard::PutNumberArray(llvm::StringRef key,
                                    llvm::ArrayRef<double> value) {
  return m_table->PutNumberArray(key, value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 * @param key the key
 * @param defaultValue the default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultNumberArray(
    llvm::StringRef key, llvm::ArrayRef<double> defaultValue) {
  return m_table->SetDefaultNumberArray(key, defaultValue);
}

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
std::vector<double> SmartDashboard::GetNumberArray(
    llvm::StringRef key, llvm::ArrayRef<double> defaultValue) {
  return m_table->GetNumberArray(key, defaultValue);
}

/**
 * Put a string array in the table
 * @param key the key to be assigned to
 * @param value the value that will be assigned
 * @return False if the table key already exists with a different type
 */
bool SmartDashboard::PutStringArray(llvm::StringRef key,
                                    llvm::ArrayRef<std::string> value) {
  return m_table->PutStringArray(key, value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 * @param key the key
 * @param defaultValue the default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultStringArray(
    llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue) {
  return m_table->SetDefaultStringArray(key, defaultValue);
}

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
std::vector<std::string> SmartDashboard::GetStringArray(
    llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue) {
  return m_table->GetStringArray(key, defaultValue);
}

/**
 * Put a raw value (byte array) in the table
 * @param key the key to be assigned to
 * @param value the value that will be assigned
 * @return False if the table key already exists with a different type
 */
bool SmartDashboard::PutRaw(llvm::StringRef key, llvm::StringRef value) {
  return m_table->PutRaw(key, value);
}

/**
 * Gets the current value in the table, setting it if it does not exist.
 * @param key the key
 * @param defaultValue the default value to set if key doesn't exist.
 * @returns False if the table key exists with a different type
 */
bool SmartDashboard::SetDefaultRaw(llvm::StringRef key,
                                   llvm::StringRef defaultValue) {
  return m_table->SetDefaultRaw(key, defaultValue);
}

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
std::string SmartDashboard::GetRaw(llvm::StringRef key,
                                   llvm::StringRef defaultValue) {
  return m_table->GetRaw(key, defaultValue);
}
