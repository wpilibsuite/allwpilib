/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Preferences.h"

#include <algorithm>

#include <HAL/HAL.h>
#include <llvm/StringRef.h>
#include <networktables/NetworkTableInstance.h>

#include "WPIErrors.h"

using namespace frc;

// The Preferences table name
static llvm::StringRef kTableName{"Preferences"};

Preferences::Preferences()
    : m_table(nt::NetworkTableInstance::GetDefault().GetTable(kTableName)) {
  m_table->GetEntry(".type").SetString("RobotPreferences");
  m_listener = m_table->AddEntryListener(
      [=](nt::NetworkTable* table, llvm::StringRef name,
          nt::NetworkTableEntry entry, std::shared_ptr<nt::Value> value,
          int flags) { entry.SetPersistent(); },
      NT_NOTIFY_NEW | NT_NOTIFY_IMMEDIATE);
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
 * The value may not have quotation marks, nor may the key have any whitespace
 * nor an equals sign.
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutString(llvm::StringRef key, llvm::StringRef value) {
  auto entry = m_table->GetEntry(key);
  entry.SetString(value);
  entry.SetPersistent();
}

/**
 * Puts the given int into the preferences table.
 *
 * The key may not have any whitespace nor an equals sign.
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutInt(llvm::StringRef key, int value) {
  auto entry = m_table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

/**
 * Puts the given double into the preferences table.
 *
 * The key may not have any whitespace nor an equals sign.
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutDouble(llvm::StringRef key, double value) {
  auto entry = m_table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

/**
 * Puts the given float into the preferences table.
 *
 * The key may not have any whitespace nor an equals sign.
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutFloat(llvm::StringRef key, float value) {
  auto entry = m_table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

/**
 * Puts the given boolean into the preferences table.
 *
 * The key may not have any whitespace nor an equals sign.
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutBoolean(llvm::StringRef key, bool value) {
  auto entry = m_table->GetEntry(key);
  entry.SetBoolean(value);
  entry.SetPersistent();
}

/**
 * Puts the given long (int64_t) into the preferences table.
 *
 * The key may not have any whitespace nor an equals sign.
 *
 * @param key   the key
 * @param value the value
 */
void Preferences::PutLong(llvm::StringRef key, int64_t value) {
  auto entry = m_table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

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
