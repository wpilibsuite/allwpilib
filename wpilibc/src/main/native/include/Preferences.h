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

#include "ErrorBase.h"

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
  static Preferences* GetInstance();

  std::vector<std::string> GetKeys();
  std::string GetString(wpi::StringRef key, wpi::StringRef defaultValue = "");
  int GetInt(wpi::StringRef key, int defaultValue = 0);
  double GetDouble(wpi::StringRef key, double defaultValue = 0.0);
  float GetFloat(wpi::StringRef key, float defaultValue = 0.0);
  bool GetBoolean(wpi::StringRef key, bool defaultValue = false);
  int64_t GetLong(wpi::StringRef key, int64_t defaultValue = 0);
  void PutString(wpi::StringRef key, wpi::StringRef value);
  void PutInt(wpi::StringRef key, int value);
  void PutDouble(wpi::StringRef key, double value);
  void PutFloat(wpi::StringRef key, float value);
  void PutBoolean(wpi::StringRef key, bool value);
  void PutLong(wpi::StringRef key, int64_t value);
  bool ContainsKey(wpi::StringRef key);
  void Remove(wpi::StringRef key);

 protected:
  Preferences();
  virtual ~Preferences() = default;

 private:
  std::shared_ptr<nt::NetworkTable> m_table;
  NT_EntryListener m_listener;
};

}  // namespace frc
