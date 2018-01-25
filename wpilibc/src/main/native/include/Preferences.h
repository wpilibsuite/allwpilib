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
  std::string GetString(llvm::StringRef key, llvm::StringRef defaultValue = "");
  int GetInt(llvm::StringRef key, int defaultValue = 0);
  double GetDouble(llvm::StringRef key, double defaultValue = 0.0);
  float GetFloat(llvm::StringRef key, float defaultValue = 0.0);
  bool GetBoolean(llvm::StringRef key, bool defaultValue = false);
  int64_t GetLong(llvm::StringRef key, int64_t defaultValue = 0);
  void PutString(llvm::StringRef key, llvm::StringRef value);
  void PutInt(llvm::StringRef key, int value);
  void PutDouble(llvm::StringRef key, double value);
  void PutFloat(llvm::StringRef key, float value);
  void PutBoolean(llvm::StringRef key, bool value);
  void PutLong(llvm::StringRef key, int64_t value);
  bool ContainsKey(llvm::StringRef key);
  void Remove(llvm::StringRef key);

 protected:
  Preferences();
  virtual ~Preferences() = default;

 private:
  std::shared_ptr<nt::NetworkTable> m_table;
  NT_EntryListener m_listener;
};

}  // namespace frc
