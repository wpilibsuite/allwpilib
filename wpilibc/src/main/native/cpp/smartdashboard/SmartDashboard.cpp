// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/SmartDashboard.h"

#include <hal/FRCUsageReporting.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/StringMap.h>
#include <wpi/mutex.h>

#include "frc/Errors.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

namespace {
class Singleton {
 public:
  static Singleton& GetInstance();

  static std::shared_ptr<nt::NetworkTable> table;
  static wpi::StringMap<SendableRegistry::UID> tablesToData;
  static wpi::mutex tablesToDataMutex;

 private:
  Singleton() {
    HAL_Report(HALUsageReporting::kResourceType_SmartDashboard, 0);
  }
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;
};

std::shared_ptr<nt::NetworkTable> Singleton::table =
    nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard");
wpi::StringMap<SendableRegistry::UID> Singleton::tablesToData;
wpi::mutex Singleton::tablesToDataMutex;

}  // namespace

Singleton& Singleton::GetInstance() {
  static Singleton instance;
  return instance;
}

void SmartDashboard::init() {
  Singleton::GetInstance();
}

bool SmartDashboard::ContainsKey(wpi::StringRef key) {
  return Singleton::table->ContainsKey(key);
}

std::vector<std::string> SmartDashboard::GetKeys(int types) {
  return Singleton::table->GetKeys(types);
}

void SmartDashboard::SetPersistent(wpi::StringRef key) {
  Singleton::table->GetEntry(key).SetPersistent();
}

void SmartDashboard::ClearPersistent(wpi::StringRef key) {
  Singleton::table->GetEntry(key).ClearPersistent();
}

bool SmartDashboard::IsPersistent(wpi::StringRef key) {
  return Singleton::table->GetEntry(key).IsPersistent();
}

void SmartDashboard::SetFlags(wpi::StringRef key, unsigned int flags) {
  Singleton::table->GetEntry(key).SetFlags(flags);
}

void SmartDashboard::ClearFlags(wpi::StringRef key, unsigned int flags) {
  Singleton::table->GetEntry(key).ClearFlags(flags);
}

unsigned int SmartDashboard::GetFlags(wpi::StringRef key) {
  return Singleton::table->GetEntry(key).GetFlags();
}

void SmartDashboard::Delete(wpi::StringRef key) {
  Singleton::table->Delete(key);
}

nt::NetworkTableEntry SmartDashboard::GetEntry(wpi::StringRef key) {
  return Singleton::table->GetEntry(key);
}

void SmartDashboard::PutData(wpi::StringRef key, Sendable* data) {
  if (!data) {
    throw FRC_MakeError(err::NullParameter, "value");
  }
  std::scoped_lock lock(Singleton::tablesToDataMutex);
  auto& uid = Singleton::tablesToData[key];
  Sendable* sddata = SendableRegistry::GetSendable(uid);
  if (sddata != data) {
    uid = SendableRegistry::GetUniqueId(data);
    auto dataTable = Singleton::table->GetSubTable(key);
    SendableRegistry::Publish(uid, dataTable);
    dataTable->GetEntry(".name").SetString(key);
  }
}

void SmartDashboard::PutData(Sendable* value) {
  if (!value) {
    throw FRC_MakeError(err::NullParameter, "value");
  }
  auto name = SendableRegistry::GetName(value);
  if (!name.empty()) {
    PutData(name, value);
  }
}

Sendable* SmartDashboard::GetData(wpi::StringRef key) {
  std::scoped_lock lock(Singleton::tablesToDataMutex);
  auto it = Singleton::tablesToData.find(key);
  if (it == Singleton::tablesToData.end()) {
    throw FRC_MakeError(err::SmartDashboardMissingKey, key);
  }
  return SendableRegistry::GetSendable(it->getValue());
}

bool SmartDashboard::PutBoolean(wpi::StringRef keyName, bool value) {
  return Singleton::table->GetEntry(keyName).SetBoolean(value);
}

bool SmartDashboard::SetDefaultBoolean(wpi::StringRef key, bool defaultValue) {
  return Singleton::table->GetEntry(key).SetDefaultBoolean(defaultValue);
}

bool SmartDashboard::GetBoolean(wpi::StringRef keyName, bool defaultValue) {
  return Singleton::table->GetEntry(keyName).GetBoolean(defaultValue);
}

bool SmartDashboard::PutNumber(wpi::StringRef keyName, double value) {
  return Singleton::table->GetEntry(keyName).SetDouble(value);
}

bool SmartDashboard::SetDefaultNumber(wpi::StringRef key, double defaultValue) {
  return Singleton::table->GetEntry(key).SetDefaultDouble(defaultValue);
}

double SmartDashboard::GetNumber(wpi::StringRef keyName, double defaultValue) {
  return Singleton::table->GetEntry(keyName).GetDouble(defaultValue);
}

bool SmartDashboard::PutString(wpi::StringRef keyName, wpi::StringRef value) {
  return Singleton::table->GetEntry(keyName).SetString(value);
}

bool SmartDashboard::SetDefaultString(wpi::StringRef key,
                                      wpi::StringRef defaultValue) {
  return Singleton::table->GetEntry(key).SetDefaultString(defaultValue);
}

std::string SmartDashboard::GetString(wpi::StringRef keyName,
                                      wpi::StringRef defaultValue) {
  return Singleton::table->GetEntry(keyName).GetString(defaultValue);
}

bool SmartDashboard::PutBooleanArray(wpi::StringRef key,
                                     wpi::ArrayRef<int> value) {
  return Singleton::table->GetEntry(key).SetBooleanArray(value);
}

bool SmartDashboard::SetDefaultBooleanArray(wpi::StringRef key,
                                            wpi::ArrayRef<int> defaultValue) {
  return Singleton::table->GetEntry(key).SetDefaultBooleanArray(defaultValue);
}

std::vector<int> SmartDashboard::GetBooleanArray(
    wpi::StringRef key, wpi::ArrayRef<int> defaultValue) {
  return Singleton::table->GetEntry(key).GetBooleanArray(defaultValue);
}

bool SmartDashboard::PutNumberArray(wpi::StringRef key,
                                    wpi::ArrayRef<double> value) {
  return Singleton::table->GetEntry(key).SetDoubleArray(value);
}

bool SmartDashboard::SetDefaultNumberArray(wpi::StringRef key,
                                           wpi::ArrayRef<double> defaultValue) {
  return Singleton::table->GetEntry(key).SetDefaultDoubleArray(defaultValue);
}

std::vector<double> SmartDashboard::GetNumberArray(
    wpi::StringRef key, wpi::ArrayRef<double> defaultValue) {
  return Singleton::table->GetEntry(key).GetDoubleArray(defaultValue);
}

bool SmartDashboard::PutStringArray(wpi::StringRef key,
                                    wpi::ArrayRef<std::string> value) {
  return Singleton::table->GetEntry(key).SetStringArray(value);
}

bool SmartDashboard::SetDefaultStringArray(
    wpi::StringRef key, wpi::ArrayRef<std::string> defaultValue) {
  return Singleton::table->GetEntry(key).SetDefaultStringArray(defaultValue);
}

std::vector<std::string> SmartDashboard::GetStringArray(
    wpi::StringRef key, wpi::ArrayRef<std::string> defaultValue) {
  return Singleton::table->GetEntry(key).GetStringArray(defaultValue);
}

bool SmartDashboard::PutRaw(wpi::StringRef key, wpi::StringRef value) {
  return Singleton::table->GetEntry(key).SetRaw(value);
}

bool SmartDashboard::SetDefaultRaw(wpi::StringRef key,
                                   wpi::StringRef defaultValue) {
  return Singleton::table->GetEntry(key).SetDefaultRaw(defaultValue);
}

std::string SmartDashboard::GetRaw(wpi::StringRef key,
                                   wpi::StringRef defaultValue) {
  return Singleton::table->GetEntry(key).GetRaw(defaultValue);
}

bool SmartDashboard::PutValue(wpi::StringRef keyName,
                              std::shared_ptr<nt::Value> value) {
  return Singleton::table->GetEntry(keyName).SetValue(value);
}

bool SmartDashboard::SetDefaultValue(wpi::StringRef key,
                                     std::shared_ptr<nt::Value> defaultValue) {
  return Singleton::table->GetEntry(key).SetDefaultValue(defaultValue);
}

std::shared_ptr<nt::Value> SmartDashboard::GetValue(wpi::StringRef keyName) {
  return Singleton::table->GetEntry(keyName).GetValue();
}

detail::ListenerExecutor SmartDashboard::listenerExecutor;

void SmartDashboard::PostListenerTask(std::function<void()> task) {
  listenerExecutor.Execute(task);
}

void SmartDashboard::UpdateValues() {
  listenerExecutor.RunListenerTasks();
  std::scoped_lock lock(Singleton::tablesToDataMutex);
  for (auto& i : Singleton::tablesToData) {
    SendableRegistry::Update(i.getValue());
  }
}
