/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "SensorBase.h"
#include "SmartDashboard/NamedSendable.h"
#include "SmartDashboard/Sendable.h"
#include "tables/ITable.h"

namespace frc {

class SmartDashboard : public SensorBase {
 public:
  static void init();

  static bool ContainsKey(llvm::StringRef key);

  static std::vector<std::string> GetKeys(int types = 0);

  static void SetPersistent(llvm::StringRef key);
  static void ClearPersistent(llvm::StringRef key);
  static bool IsPersistent(llvm::StringRef key);

  static void SetFlags(llvm::StringRef key, unsigned int flags);
  static void ClearFlags(llvm::StringRef key, unsigned int flags);
  static unsigned int GetFlags(llvm::StringRef key);

  static void Delete(llvm::StringRef key);

  static void PutData(llvm::StringRef key, Sendable* data);
  static void PutData(NamedSendable* value);
  static Sendable* GetData(llvm::StringRef keyName);

  static bool PutBoolean(llvm::StringRef keyName, bool value);
  static bool SetDefaultBoolean(llvm::StringRef key, bool defaultValue);
  static bool GetBoolean(llvm::StringRef keyName, bool defaultValue);

  static bool PutNumber(llvm::StringRef keyName, double value);
  static bool SetDefaultNumber(llvm::StringRef key, double defaultValue);
  static double GetNumber(llvm::StringRef keyName, double defaultValue);

  static bool PutString(llvm::StringRef keyName, llvm::StringRef value);
  static bool SetDefaultString(llvm::StringRef key,
                               llvm::StringRef defaultValue);
  static std::string GetString(llvm::StringRef keyName,
                               llvm::StringRef defaultValue);

  static bool PutBooleanArray(llvm::StringRef key, llvm::ArrayRef<int> value);
  static bool SetDefaultBooleanArray(llvm::StringRef key,
                                     llvm::ArrayRef<int> defaultValue);
  static std::vector<int> GetBooleanArray(llvm::StringRef key,
                                          llvm::ArrayRef<int> defaultValue);

  static bool PutNumberArray(llvm::StringRef key, llvm::ArrayRef<double> value);
  static bool SetDefaultNumberArray(llvm::StringRef key,
                                    llvm::ArrayRef<double> defaultValue);
  static std::vector<double> GetNumberArray(
      llvm::StringRef key, llvm::ArrayRef<double> defaultValue);

  static bool PutStringArray(llvm::StringRef key,
                             llvm::ArrayRef<std::string> value);
  static bool SetDefaultStringArray(llvm::StringRef key,
                                    llvm::ArrayRef<std::string> defaultValue);
  static std::vector<std::string> GetStringArray(
      llvm::StringRef key, llvm::ArrayRef<std::string> defaultValue);

  static bool PutRaw(llvm::StringRef key, llvm::StringRef value);
  static bool SetDefaultRaw(llvm::StringRef key, llvm::StringRef defaultValue);
  static std::string GetRaw(llvm::StringRef key, llvm::StringRef defaultValue);

  static bool PutValue(llvm::StringRef keyName,
                       std::shared_ptr<nt::Value> value);
  static bool SetDefaultValue(llvm::StringRef key,
                              std::shared_ptr<nt::Value> defaultValue);
  static std::shared_ptr<nt::Value> GetValue(llvm::StringRef keyName);

 private:
  virtual ~SmartDashboard() = default;

  /** The {@link NetworkTable} used by {@link SmartDashboard} */
  static std::shared_ptr<ITable> m_table;

  /**
   * A map linking tables in the SmartDashboard to the
   * {@link SmartDashboardData} objects they came from.
   */
  static std::map<std::shared_ptr<ITable>, Sendable*> m_tablesToData;
};

}  // namespace frc
