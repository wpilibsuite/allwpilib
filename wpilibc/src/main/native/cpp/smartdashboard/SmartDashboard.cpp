/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/SmartDashboard.h"

#include <hal/FRCUsageReporting.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/StringMap.h>
#include <wpi/mutex.h>

#include "frc/WPIErrors.h"
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

void SmartDashboard::init() { Singleton::GetInstance(); }

bool SmartDashboard::ContainsKey(wpi::StringRef key) {
  return Singleton::GetInstance().table->ContainsKey(key);
}

std::vector<std::string> SmartDashboard::GetKeys(int types) {
  return Singleton::GetInstance().table->GetKeys(types);
}

void SmartDashboard::SetPersistent(wpi::StringRef key) {
  Singleton::GetInstance().table->GetEntry(key).SetPersistent();
}

void SmartDashboard::ClearPersistent(wpi::StringRef key) {
  Singleton::GetInstance().table->GetEntry(key).ClearPersistent();
}

bool SmartDashboard::IsPersistent(wpi::StringRef key) {
  return Singleton::GetInstance().table->GetEntry(key).IsPersistent();
}

void SmartDashboard::SetFlags(wpi::StringRef key, unsigned int flags) {
  Singleton::GetInstance().table->GetEntry(key).SetFlags(flags);
}

void SmartDashboard::ClearFlags(wpi::StringRef key, unsigned int flags) {
  Singleton::GetInstance().table->GetEntry(key).ClearFlags(flags);
}

unsigned int SmartDashboard::GetFlags(wpi::StringRef key) {
  return Singleton::GetInstance().table->GetEntry(key).GetFlags();
}

void SmartDashboard::Delete(wpi::StringRef key) {
  Singleton::GetInstance().table->Delete(key);
}

nt::NetworkTableEntry SmartDashboard::GetEntry(wpi::StringRef key) {
  return Singleton::GetInstance().table->GetEntry(key);
}

void SmartDashboard::PutData(wpi::StringRef key, Sendable* data) {
  if (data == nullptr) {
    wpi_setGlobalWPIErrorWithContext(NullParameter, "value");
    return;
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
  if (value == nullptr) {
    wpi_setGlobalWPIErrorWithContext(NullParameter, "value");
    return;
  }
  auto name = SendableRegistry::GetInstance().GetName(value);
  if (!name.empty()) PutData(name, value);
}

Sendable* SmartDashboard::GetData(wpi::StringRef key) {
  auto& inst = Singleton::GetInstance();
  std::scoped_lock lock(inst.tablesToDataMutex);
  auto it = inst.tablesToData.find(key);
  if (it == inst.tablesToData.end()) {
    wpi_setGlobalWPIErrorWithContext(SmartDashboardMissingKey, key);
    return nullptr;
  }
  return SendableRegistry::GetInstance().GetSendable(it->getValue());
}

bool SmartDashboard::PutBoolean(wpi::StringRef keyName, bool value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetBoolean(value);
}

bool SmartDashboard::SetDefaultBoolean(wpi::StringRef key, bool defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultBoolean(
      defaultValue);
}

bool SmartDashboard::GetBoolean(wpi::StringRef keyName, bool defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetBoolean(
      defaultValue);
}

bool SmartDashboard::PutNumber(wpi::StringRef keyName, double value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetDouble(value);
}

bool SmartDashboard::SetDefaultNumber(wpi::StringRef key, double defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultDouble(
      defaultValue);
}

double SmartDashboard::GetNumber(wpi::StringRef keyName, double defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetDouble(
      defaultValue);
}

bool SmartDashboard::PutString(wpi::StringRef keyName, wpi::StringRef value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetString(value);
}

bool SmartDashboard::SetDefaultString(wpi::StringRef key,
                                      wpi::StringRef defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultString(
      defaultValue);
}

std::string SmartDashboard::GetString(wpi::StringRef keyName,
                                      wpi::StringRef defaultValue) {
  return Singleton::GetInstance().table->GetEntry(keyName).GetString(
      defaultValue);
}

bool SmartDashboard::PutBooleanArray(wpi::StringRef key,
                                     wpi::ArrayRef<int> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetBooleanArray(value);
}

bool SmartDashboard::SetDefaultBooleanArray(wpi::StringRef key,
                                            wpi::ArrayRef<int> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultBooleanArray(
      defaultValue);
}

std::vector<int> SmartDashboard::GetBooleanArray(
    wpi::StringRef key, wpi::ArrayRef<int> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetBooleanArray(
      defaultValue);
}

bool SmartDashboard::PutNumberArray(wpi::StringRef key,
                                    wpi::ArrayRef<double> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetDoubleArray(value);
}

bool SmartDashboard::SetDefaultNumberArray(wpi::StringRef key,
                                           wpi::ArrayRef<double> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultDoubleArray(
      defaultValue);
}

std::vector<double> SmartDashboard::GetNumberArray(
    wpi::StringRef key, wpi::ArrayRef<double> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetDoubleArray(
      defaultValue);
}

bool SmartDashboard::PutStringArray(wpi::StringRef key,
                                    wpi::ArrayRef<std::string> value) {
  return Singleton::GetInstance().table->GetEntry(key).SetStringArray(value);
}

bool SmartDashboard::SetDefaultStringArray(
    wpi::StringRef key, wpi::ArrayRef<std::string> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultStringArray(
      defaultValue);
}

std::vector<std::string> SmartDashboard::GetStringArray(
    wpi::StringRef key, wpi::ArrayRef<std::string> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetStringArray(
      defaultValue);
}

bool SmartDashboard::PutRaw(wpi::StringRef key, wpi::StringRef value) {
  return Singleton::GetInstance().table->GetEntry(key).SetRaw(value);
}

bool SmartDashboard::SetDefaultRaw(wpi::StringRef key,
                                   wpi::StringRef defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultRaw(
      defaultValue);
}

std::string SmartDashboard::GetRaw(wpi::StringRef key,
                                   wpi::StringRef defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).GetRaw(defaultValue);
}

bool SmartDashboard::PutValue(wpi::StringRef keyName,
                              std::shared_ptr<nt::Value> value) {
  return Singleton::GetInstance().table->GetEntry(keyName).SetValue(value);
}

bool SmartDashboard::SetDefaultValue(wpi::StringRef key,
                                     std::shared_ptr<nt::Value> defaultValue) {
  return Singleton::GetInstance().table->GetEntry(key).SetDefaultValue(
      defaultValue);
}

std::shared_ptr<nt::Value> SmartDashboard::GetValue(wpi::StringRef keyName) {
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
  for (auto& i : inst.tablesToData) registry.Update(i.getValue());
}
