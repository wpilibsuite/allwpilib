/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "ErrorBase.h"
#include "networktables/NetworkTable.h"
#include "tables/ITableListener.h"

namespace frc {

/**
 * The preferences class provides a relatively simple way to save important
 * values to the roboRIO to access the next time the roboRIO is booted.
 *
 * <p>This class loads and saves from a file inside the roboRIO.  The user can
 * not access the file directly, but may modify values at specific fields which
 * will then be automatically periodically saved to the file by the NetworkTable
 * server.</p>
 *
 * <p>This class is thread safe.</p>
 *
 * <p>This will also interact with {@link NetworkTable} by creating a table
 * called "Preferences" with all the key-value pairs.</p>
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
  std::shared_ptr<ITable> m_table;
  class Listener : public ITableListener {
   public:
    void ValueChanged(ITable* source, llvm::StringRef key,
                      std::shared_ptr<nt::Value> value, bool isNew) override;
    void ValueChangedEx(ITable* source, llvm::StringRef key,
                        std::shared_ptr<nt::Value> value,
                        uint32_t flags) override;
  };
  Listener m_listener;
};

}  // namespace frc
