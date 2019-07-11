/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include <networktables/NetworkTable.h>

#include "frc/ErrorBase.h"

namespace frc {

/**
 * The preferences class provides a relatively simple way to save important
 * values to the roboRIO to access the next time the roboRIO is booted.
 *
 * This class loads and saves from a file inside the roboRIO.  The user cannot
 * access the file directly, but may modify values at specific fields which will
 * then be automatically periodically saved to the file by the NetworkTable
 * server.
 *
 * This class is thread safe.
 *
 * This will also interact with {@link NetworkTable} by creating a table called
 * "Preferences" with all the key-value pairs.
 */
class Preferences : public ErrorBase {
 public:
  /**
   * Get the one and only {@link Preferences} object.
   *
   * @return pointer to the {@link Preferences}
   */
  static Preferences* GetInstance();

  /**
   * Returns a vector of all the keys.
   *
   * @return a vector of the keys
   */
  std::vector<std::string> GetKeys();

  /**
   * Returns the string at the given key.  If this table does not have a value
   * for that position, then the given defaultValue will be returned.
   *
   * @param key          the key
   * @param defaultValue the value to return if none exists in the table
   * @return either the value in the table, or the defaultValue
   */
  std::string GetString(wpi::StringRef key, wpi::StringRef defaultValue = "");

  /**
   * Returns the int at the given key.  If this table does not have a value for
   * that position, then the given defaultValue value will be returned.
   *
   * @param key          the key
   * @param defaultValue the value to return if none exists in the table
   * @return either the value in the table, or the defaultValue
   */
  int GetInt(wpi::StringRef key, int defaultValue = 0);

  /**
   * Returns the double at the given key.  If this table does not have a value
   * for that position, then the given defaultValue value will be returned.
   *
   * @param key          the key
   * @param defaultValue the value to return if none exists in the table
   * @return either the value in the table, or the defaultValue
   */
  double GetDouble(wpi::StringRef key, double defaultValue = 0.0);

  /**
   * Returns the float at the given key.  If this table does not have a value
   * for that position, then the given defaultValue value will be returned.
   *
   * @param key          the key
   * @param defaultValue the value to return if none exists in the table
   * @return either the value in the table, or the defaultValue
   */
  float GetFloat(wpi::StringRef key, float defaultValue = 0.0);

  /**
   * Returns the boolean at the given key.  If this table does not have a value
   * for that position, then the given defaultValue value will be returned.
   *
   * @param key          the key
   * @param defaultValue the value to return if none exists in the table
   * @return either the value in the table, or the defaultValue
   */
  bool GetBoolean(wpi::StringRef key, bool defaultValue = false);

  /**
   * Returns the long (int64_t) at the given key.  If this table does not have a
   * value for that position, then the given defaultValue value will be
   * returned.
   *
   * @param key          the key
   * @param defaultValue the value to return if none exists in the table
   * @return either the value in the table, or the defaultValue
   */
  int64_t GetLong(wpi::StringRef key, int64_t defaultValue = 0);

  /**
   * Puts the given string into the preferences table.
   *
   * The value may not have quotation marks, nor may the key have any whitespace
   * nor an equals sign.
   *
   * @param key   the key
   * @param value the value
   */
  void PutString(wpi::StringRef key, wpi::StringRef value);

  /**
   * Puts the given int into the preferences table.
   *
   * The key may not have any whitespace nor an equals sign.
   *
   * @param key   the key
   * @param value the value
   */
  void PutInt(wpi::StringRef key, int value);

  /**
   * Puts the given double into the preferences table.
   *
   * The key may not have any whitespace nor an equals sign.
   *
   * @param key   the key
   * @param value the value
   */
  void PutDouble(wpi::StringRef key, double value);

  /**
   * Puts the given float into the preferences table.
   *
   * The key may not have any whitespace nor an equals sign.
   *
   * @param key   the key
   * @param value the value
   */
  void PutFloat(wpi::StringRef key, float value);

  /**
   * Puts the given boolean into the preferences table.
   *
   * The key may not have any whitespace nor an equals sign.
   *
   * @param key   the key
   * @param value the value
   */
  void PutBoolean(wpi::StringRef key, bool value);

  /**
   * Puts the given long (int64_t) into the preferences table.
   *
   * The key may not have any whitespace nor an equals sign.
   *
   * @param key   the key
   * @param value the value
   */
  void PutLong(wpi::StringRef key, int64_t value);

  /**
   * Returns whether or not there is a key with the given name.
   *
   * @param key the key
   * @return if there is a value at the given key
   */
  bool ContainsKey(wpi::StringRef key);

  /**
   * Remove a preference.
   *
   * @param key the key
   */
  void Remove(wpi::StringRef key);

  /**
   * Remove all preferences.
   */
  void RemoveAll();

 protected:
  Preferences();
  virtual ~Preferences() = default;

  Preferences(Preferences&&) = default;
  Preferences& operator=(Preferences&&) = default;

 private:
  std::shared_ptr<nt::NetworkTable> m_table;
  NT_EntryListener m_listener;
};

}  // namespace frc
