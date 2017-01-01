/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Preferences.h"

#include <algorithm>

#include "HAL/HAL.h"
#include "WPIErrors.h"
#include "llvm/StringRef.h"

using namespace frc;

/** The Preferences table name */
static llvm::StringRef kTableName{"Preferences"};

void Preferences::Listener::ValueChanged(ITable* source, llvm::StringRef key,
                                         std::shared_ptr<nt::Value> value,
                                         bool isNew) {}
void Preferences::Listener::ValueChangedEx(ITable* source, llvm::StringRef key,
                                           std::shared_ptr<nt::Value> value,
                                           uint32_t flags) {
  source->SetPersistent(key);
}

Preferences::Preferences() : m_table(NetworkTable::GetTable(kTableName)) {
  m_table->AddTableListenerEx(&m_listener, NT_NOTIFY_NEW | NT_NOTIFY_IMMEDIATE);
  HAL_Report(HALUsageReporting::kResourceType_Preferences, 0);
}

/**
 * Get the one and only {@link Preferences} object.
 *
 * @return pointer to the {@link Preferences}
 */
Preferences* Preferences::GetInstance() {
  static Preferences instance;
  return &instance;
}

/**
 * Returns a vector of all the keys.
 *
 * @return a vector of the keys
 */
std::vector<std::string> Preferences::GetKeys() { return m_table->GetKeys(); }

/**
 * Returns the string at the given key.  If this table does not have a value
 * for that position, then the given defaultValue will be returned.
 *
 * @param key          the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
std::string Preferences::GetString(llvm::StringRef key,
                                   llvm::StringRef defaultValue) {
  return m_table->GetString(key, defaultValue);
}

/**
 * Returns the int at the given key.  If this table does not have a value for
 * that position, then the given defaultValue value will be returned.
 *
 * @param key          the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
int Preferences::GetInt(llvm::StringRef key, int defaultValue) {
  return static_cast<int>(m_table->GetNumber(key, defaultValue));
}

/**
 * Returns the double at the given key.  If this table does not have a value
 * for that position, then the given defaultValue value will be returned.
 *
 * @param key          the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
double Preferences::GetDouble(llvm::StringRef key, double defaultValue) {
  return m_table->GetNumber(key, defaultValue);
}

/**
 * Returns the float at the given key.  If this table does not have a value
 * for that position, then the given defaultValue value will be returned.
 *
 * @param key          the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
float Preferences::GetFloat(llvm::StringRef key, float defaultValue) {
  return m_table->GetNumber(key, defaultValue);
}

/**
 * Returns the boolean at the given key.  If this table does not have a value
 * for that position, then the given defaultValue value will be returned.
 *
 * @param key          the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
bool Preferences::GetBoolean(llvm::StringRef key, bool defaultValue) {
  return m_table->GetBoolean(key, defaultValue);
}

/**
 * Returns the long (int64_t) at the given key.  If this table does not have a
 * value for that position, then the given defaultValue value will be returned.
 *
 * @param key          the key
 * @param defaultValue the value to return if none exists in the table
 * @return either the value in the table, or the defaultValue
 */
int64_t Preferences::GetLong(llvm::StringRef key, int64_t defaultValue) {
  return static_cast<int64_t>(m_table->GetNumber(key, defaultValue));
}

/**
 * Puts the given string into the preferences table.
 *
 * <p>The value may not have quotation marks, nor may the key
 * have any whitespace nor an equals sign</p>
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutString(llvm::StringRef key, llvm::StringRef value) {
  m_table->PutString(key, value);
  m_table->SetPersistent(key);
}

/**
 * Puts the given int into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutInt(llvm::StringRef key, int value) {
  m_table->PutNumber(key, value);
  m_table->SetPersistent(key);
}

/**
 * Puts the given double into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutDouble(llvm::StringRef key, double value) {
  m_table->PutNumber(key, value);
  m_table->SetPersistent(key);
}

/**
 * Puts the given float into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutFloat(llvm::StringRef key, float value) {
  m_table->PutNumber(key, value);
  m_table->SetPersistent(key);
}

/**
 * Puts the given boolean into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutBoolean(llvm::StringRef key, bool value) {
  m_table->PutBoolean(key, value);
  m_table->SetPersistent(key);
}

/**
 * Puts the given long (int64_t) into the preferences table.
 *
 * <p>The key may not have any whitespace nor an equals sign</p>
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutLong(llvm::StringRef key, int64_t value) {
  m_table->PutNumber(key, value);
  m_table->SetPersistent(key);
}

/**
 * This function is no longer required, as NetworkTables automatically
 * saves persistent values (which all Preferences values are) periodically
 * when running as a server.
 * @deprecated backwards compatibility shim
 */
void Preferences::Save() {}

/**
 * Returns whether or not there is a key with the given name.
 *
 * @param key the key
 * @return if there is a value at the given key
 */
bool Preferences::ContainsKey(llvm::StringRef key) {
  return m_table->ContainsKey(key);
}

/**
 * Remove a preference.
 *
 * @param key the key
 */
void Preferences::Remove(llvm::StringRef key) { m_table->Delete(key); }
