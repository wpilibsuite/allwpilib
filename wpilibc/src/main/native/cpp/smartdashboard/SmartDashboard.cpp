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

  std::shared_ptr<nt::NetworkTable> table;
  wpi::StringMap<SendableRegistry::UID> tablesToData;
  wpi::mutex tablesToDataMutex;

 private:
  Singleton() {
    table = nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard");
    HAL_Report(HALUsageReporting::kResourceType_SmartDashboard, 0);
  }
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;
};

}  // namespace

Singleton& Singleton::GetInstance() {
  static Singleton instance;
  return instance;
}

void SmartDashboard::init() {
  Singleton::GetInstance();
}

bool SmartDashboard::ContainsKey(std::string_view key) {
  return Singleton::GetInstance().table->ContainsKey(key);
}

std::vector<std::string> SmartDashboard::GetKeys(int types) {
  return Singleton::GetInstance().table->GetKeys(types);
}

void SmartDashboard::SetPersistent(std::string_view key) {
  Singleton::GetInstance().table->GetEntry(key).SetPersistent();
}

void SmartDashboard::ClearPersistent(std::string_view key) {
  Singleton::GetInstance().table->GetEntry(key).ClearPersistent();
}

bool SmartDashboard::IsPersistent(std::string_view key) {
  return Singleton::GetInstance().table->GetEntry(key).IsPersistent();
}

void SmartDashboard::SetFlags(std::string_view key, unsigned int flags) {
  Singleton::GetInstance().table->GetEntry(key).SetFlags(flags);
}

void SmartDashboard::ClearFlags(std::string_view key, unsigned int flags) {
  Singleton::GetInstance().table->GetEntry(key).ClearFlags(flags);
}

unsigned int SmartDashboard::GetFlags(std::string_view key) {
  return Singleton::GetInstance().table->GetEntry(key).GetFlags();
}

void SmartDashboard::Delete(std::string_view key) {
  Singleton::GetInstance().table->Delete(key);
}

nt::NetworkTableEntry SmartDashboard::GetEntry(std::string_view key) {
  return Singleton::GetInstance().table->GetEntry(key);
}

void SmartDashboard::PutData(std::string_view key, Sendable* data) {
  if (!data) {
    throw FRC_MakeError(err::NullParameter, "{}", "value");
  }
  auto& inst = Singleton::GetInstance();
  std::scoped_lock lock(inst.tablesToDataMutex);
  auto& uid = inst.tablesToData[key];
  auto& registry = SendableRegistry::GetInstance();
  Sendable* sddata = registry.GetSendable(uid);
  if (sddata != data) {
    uid = registry.GetUniqueId(data);
    auto dataTable = inst.table->GetSubTable(key);
    registry.Publish(uid, dataTable);
    dataTable->GetEntry(".name").SetString(key);
  }
}

void SmartDashboard::PutData(Sendable* value) {
  if (!value) {
    throw FRC_MakeError(err::NullParameter, "{}", "value");
  }
  auto name = SendableRegistry::GetInstance().GetName(value);
  if (!name.empty()) {
    PutData(name, value);
  }
}

Sendable* SmartDashboard::GetData(std::string_view key) {
  auto& inst = Singleton::GetInstance();
  std::scoped_lock lock(inst.tablesToDataMutex);
  auto it = inst.tablesToData.find(key);
  if (it == inst.tablesToData.end()) {
    throw FRC_MakeError(err::SmartDashboardMissingKey, "{}", key);
  }
  return SendableRegistry::GetInstance().GetSendable(it->getValue());
}

bool SmartDashboard::PutBoolean(std::string_view keyName, bool value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetBoolean(value);
}

bool SmartDashboard::SetDefaultBoolean(std::string_view key,
                                       bool defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultBoolean(
      defaultValue);
}

bool SmartDashboard::GetBoolean(std::string_view keyName, bool defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetBoolean(
      defaultValue);
}

bool SmartDashboard::PutNumber(std::string_view keyName, double value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetDouble(value);
}

bool SmartDashboard::SetDefaultNumber(std::string_view key,
                                      double defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultDouble(
      defaultValue);
}

double SmartDashboard::GetNumber(std::string_view keyName,
                                 double defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetDouble(
      defaultValue);
}

bool SmartDashboard::PutString(std::string_view keyName,
                               std::string_view value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetString(value);
}

bool SmartDashboard::SetDefaultString(std::string_view key,
                                      std::string_view defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultString(
      defaultValue);
}

std::string SmartDashboard::GetString(std::string_view keyName,
                                      std::string_view defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetString(
      defaultValue);
}

bool SmartDashboard::PutBooleanArray(std::string_view key,
                                     wpi::span<const int> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetBooleanArray(value);
}

bool SmartDashboard::SetDefaultBooleanArray(std::string_view key,
                                            wpi::span<const int> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultBooleanArray(
      defaultValue);
}

std::vector<int> SmartDashboard::GetBooleanArray(
    std::string_view key, wpi::span<const int> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetBooleanArray(
      defaultValue);
}

bool SmartDashboard::PutNumberArray(std::string_view key,
                                    wpi::span<const double> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetDoubleArray(value);
}

bool SmartDashboard::SetDefaultNumberArray(
    std::string_view key, wpi::span<const double> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultDoubleArray(
      defaultValue);
}

std::vector<double> SmartDashboard::GetNumberArray(
    std::string_view key, wpi::span<const double> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetDoubleArray(
      defaultValue);
}

bool SmartDashboard::PutStringArray(std::string_view key,
                                    wpi::span<const std::string> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetStringArray(value);
}

bool SmartDashboard::SetDefaultStringArray(
    std::string_view key, wpi::span<const std::string> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultStringArray(
      defaultValue);
}

std::vector<std::string> SmartDashboard::GetStringArray(
    std::string_view key, wpi::span<const std::string> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetStringArray(
      defaultValue);
}

bool SmartDashboard::PutRaw(std::string_view key, std::string_view value) {
  return Singleton::GetInstance().table->GetEntry(key).SetRaw(value);
}

bool SmartDashboard::SetDefaultRaw(std::string_view key,
                                   std::string_view defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultRaw(
      defaultValue);
}

std::string SmartDashboard::GetRaw(std::string_view key,
                                   std::string_view defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetRaw(defaultValue);
}

bool SmartDashboard::PutValue(std::string_view keyName,
                              std::shared_ptr<nt::Value> value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetValue(value);
}

bool SmartDashboard::SetDefaultValue(std::string_view key,
                                     std::shared_ptr<nt::Value> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultValue(
      defaultValue);
}

std::shared_ptr<nt::Value> SmartDashboard::GetValue(std::string_view keyName) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetValue();
}

detail::ListenerExecutor SmartDashboard::listenerExecutor;

void SmartDashboard::PostListenerTask(std::function<void()> task) {
  listenerExecutor.Execute(task);
}

void SmartDashboard::UpdateValues() {
  auto& registry = SendableRegistry::GetInstance();
  auto& inst = Singleton::GetInstance();
  listenerExecutor.RunListenerTasks();
  std::scoped_lock lock(inst.tablesToDataMutex);
  for (auto& i : inst.tablesToData) {
    registry.Update(i.getValue());
  }
}
