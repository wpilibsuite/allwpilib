/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <networktables/NetworkTableValue.h>

#include "SensorBase.h"

namespace frc {

class Sendable;

class SmartDashboard : public SensorBase {
 public:
  static void init();

  static bool ContainsKey(wpi::StringRef key);

  static std::vector<std::string> GetKeys(int types = 0);

  static void SetPersistent(wpi::StringRef key);
  static void ClearPersistent(wpi::StringRef key);
  static bool IsPersistent(wpi::StringRef key);

  static void SetFlags(wpi::StringRef key, unsigned int flags);
  static void ClearFlags(wpi::StringRef key, unsigned int flags);
  static unsigned int GetFlags(wpi::StringRef key);

  static void Delete(wpi::StringRef key);

  static void PutData(wpi::StringRef key, Sendable* data);
  static void PutData(Sendable* value);
  static Sendable* GetData(wpi::StringRef keyName);

  static bool PutBoolean(wpi::StringRef keyName, bool value);
  static bool SetDefaultBoolean(wpi::StringRef key, bool defaultValue);
  static bool GetBoolean(wpi::StringRef keyName, bool defaultValue);

  static bool PutNumber(wpi::StringRef keyName, double value);
  static bool SetDefaultNumber(wpi::StringRef key, double defaultValue);
  static double GetNumber(wpi::StringRef keyName, double defaultValue);

  static bool PutString(wpi::StringRef keyName, wpi::StringRef value);
  static bool SetDefaultString(wpi::StringRef key,
                               wpi::StringRef defaultValue);
  static std::string GetString(wpi::StringRef keyName,
                               wpi::StringRef defaultValue);

  static bool PutBooleanArray(wpi::StringRef key, wpi::ArrayRef<int> value);
  static bool SetDefaultBooleanArray(wpi::StringRef key,
                                     wpi::ArrayRef<int> defaultValue);
  static std::vector<int> GetBooleanArray(wpi::StringRef key,
                                          wpi::ArrayRef<int> defaultValue);

  static bool PutNumberArray(wpi::StringRef key, wpi::ArrayRef<double> value);
  static bool SetDefaultNumberArray(wpi::StringRef key,
                                    wpi::ArrayRef<double> defaultValue);
  static std::vector<double> GetNumberArray(
      wpi::StringRef key, wpi::ArrayRef<double> defaultValue);

  static bool PutStringArray(wpi::StringRef key,
                             wpi::ArrayRef<std::string> value);
  static bool SetDefaultStringArray(wpi::StringRef key,
                                    wpi::ArrayRef<std::string> defaultValue);
  static std::vector<std::string> GetStringArray(
      wpi::StringRef key, wpi::ArrayRef<std::string> defaultValue);

  static bool PutRaw(wpi::StringRef key, wpi::StringRef value);
  static bool SetDefaultRaw(wpi::StringRef key, wpi::StringRef defaultValue);
  static std::string GetRaw(wpi::StringRef key, wpi::StringRef defaultValue);

  static bool PutValue(wpi::StringRef keyName,
                       std::shared_ptr<nt::Value> value);
  static bool SetDefaultValue(wpi::StringRef key,
                              std::shared_ptr<nt::Value> defaultValue);
  static std::shared_ptr<nt::Value> GetValue(wpi::StringRef keyName);

  static void UpdateValues();

 private:
  virtual ~SmartDashboard() = default;
};

}  // namespace frc
