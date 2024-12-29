// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/SmartDashboard.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <hal/FRCUsageReporting.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/StringMap.h>
#include <wpi/mutex.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/smartdashboard/ListenerExecutor.h"
#include "frc/smartdashboard/SendableBuilderImpl.h"

using namespace frc;

namespace {
struct Instance {
  detail::ListenerExecutor listenerExecutor;
  std::shared_ptr<nt::NetworkTable> table =
      nt::NetworkTableInstance::GetDefault().GetTable("SmartDashboard");
  wpi::StringMap<wpi::SendableRegistry::UID> tablesToData;
  wpi::mutex tablesToDataMutex;
};
}  // namespace

static std::unique_ptr<Instance>& GetInstanceHolder() {
  static std::unique_ptr<Instance> instance = std::make_unique<Instance>();
  return instance;
}

static Instance& GetInstance() {
  return *GetInstanceHolder();
}

#ifndef __FRC_ROBORIO__
namespace frc::impl {
void ResetSmartDashboardInstance() {
  std::make_unique<Instance>().swap(GetInstanceHolder());
}
}  // namespace frc::impl
#endif

static bool gReported = false;

void SmartDashboard::init() {
  GetInstance();
}

bool SmartDashboard::ContainsKey(std::string_view key) {
  return GetInstance().table->ContainsKey(key);
}

std::vector<std::string> SmartDashboard::GetKeys(int types) {
  return GetInstance().table->GetKeys(types);
}

void SmartDashboard::SetPersistent(std::string_view key) {
  GetEntry(key).SetPersistent();
}

void SmartDashboard::ClearPersistent(std::string_view key) {
  GetEntry(key).ClearPersistent();
}

bool SmartDashboard::IsPersistent(std::string_view key) {
  return GetEntry(key).IsPersistent();
}

nt::NetworkTableEntry SmartDashboard::GetEntry(std::string_view key) {
  if (!gReported) {
    HAL_Report(HALUsageReporting::kResourceType_SmartDashboard,
               HALUsageReporting::kSmartDashboard_Instance);
    gReported = true;
  }
  return GetInstance().table->GetEntry(key);
}

void SmartDashboard::PutData(std::string_view key, wpi::Sendable* data) {
  if (!data) {
    throw FRC_MakeError(err::NullParameter, "value");
  }
  if (!gReported) {
    HAL_Report(HALUsageReporting::kResourceType_SmartDashboard,
               HALUsageReporting::kSmartDashboard_Instance);
    gReported = true;
  }
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.tablesToDataMutex);
  auto& uid = inst.tablesToData[key];
  wpi::Sendable* sddata = wpi::SendableRegistry::GetSendable(uid);
  if (sddata != data) {
    uid = wpi::SendableRegistry::GetUniqueId(data);
    auto dataTable = inst.table->GetSubTable(key);
    auto builder = std::make_unique<SendableBuilderImpl>();
    auto builderPtr = builder.get();
    builderPtr->SetTable(dataTable);
    wpi::SendableRegistry::Publish(uid, std::move(builder));
    builderPtr->StartListeners();
    dataTable->GetEntry(".name").SetString(key);
  }
}

void SmartDashboard::PutData(wpi::Sendable* value) {
  if (!value) {
    throw FRC_MakeError(err::NullParameter, "value");
  }
  auto name = wpi::SendableRegistry::GetName(value);
  if (!name.empty()) {
    PutData(name, value);
  }
}

wpi::Sendable* SmartDashboard::GetData(std::string_view key) {
  auto& inst = GetInstance();
  std::scoped_lock lock(inst.tablesToDataMutex);
  auto it = inst.tablesToData.find(key);
  if (it == inst.tablesToData.end()) {
    throw FRC_MakeError(err::SmartDashboardMissingKey, "{}", key);
  }
  return wpi::SendableRegistry::GetSendable(it->second);
}

bool SmartDashboard::PutBoolean(std::string_view keyName, bool value) {
  return GetInstance().table->GetEntry(keyName).SetBoolean(value);
}

bool SmartDashboard::SetDefaultBoolean(std::string_view key,
                                       bool defaultValue) {
  return GetEntry(key).SetDefaultBoolean(defaultValue);
}

bool SmartDashboard::GetBoolean(std::string_view keyName, bool defaultValue) {
  return GetInstance().table->GetEntry(keyName).GetBoolean(defaultValue);
}

bool SmartDashboard::PutNumber(std::string_view keyName, double value) {
  return GetInstance().table->GetEntry(keyName).SetDouble(value);
}

bool SmartDashboard::SetDefaultNumber(std::string_view key,
                                      double defaultValue) {
  return GetEntry(key).SetDefaultDouble(defaultValue);
}

double SmartDashboard::GetNumber(std::string_view keyName,
                                 double defaultValue) {
  return GetInstance().table->GetEntry(keyName).GetDouble(defaultValue);
}

bool SmartDashboard::PutString(std::string_view keyName,
                               std::string_view value) {
  return GetInstance().table->GetEntry(keyName).SetString(value);
}

bool SmartDashboard::SetDefaultString(std::string_view key,
                                      std::string_view defaultValue) {
  return GetEntry(key).SetDefaultString(defaultValue);
}

std::string SmartDashboard::GetString(std::string_view keyName,
                                      std::string_view defaultValue) {
  return GetInstance().table->GetEntry(keyName).GetString(defaultValue);
}

bool SmartDashboard::PutBooleanArray(std::string_view key,
                                     std::span<const int> value) {
  return GetEntry(key).SetBooleanArray(value);
}

bool SmartDashboard::SetDefaultBooleanArray(std::string_view key,
                                            std::span<const int> defaultValue) {
  return GetEntry(key).SetDefaultBooleanArray(defaultValue);
}

std::vector<int> SmartDashboard::GetBooleanArray(
    std::string_view key, std::span<const int> defaultValue) {
  return GetEntry(key).GetBooleanArray(defaultValue);
}

bool SmartDashboard::PutNumberArray(std::string_view key,
                                    std::span<const double> value) {
  return GetEntry(key).SetDoubleArray(value);
}

bool SmartDashboard::SetDefaultNumberArray(
    std::string_view key, std::span<const double> defaultValue) {
  return GetEntry(key).SetDefaultDoubleArray(defaultValue);
}

std::vector<double> SmartDashboard::GetNumberArray(
    std::string_view key, std::span<const double> defaultValue) {
  return GetEntry(key).GetDoubleArray(defaultValue);
}

bool SmartDashboard::PutStringArray(std::string_view key,
                                    std::span<const std::string> value) {
  return GetEntry(key).SetStringArray(value);
}

bool SmartDashboard::SetDefaultStringArray(
    std::string_view key, std::span<const std::string> defaultValue) {
  return GetEntry(key).SetDefaultStringArray(defaultValue);
}

std::vector<std::string> SmartDashboard::GetStringArray(
    std::string_view key, std::span<const std::string> defaultValue) {
  return GetEntry(key).GetStringArray(defaultValue);
}

bool SmartDashboard::PutRaw(std::string_view key,
                            std::span<const uint8_t> value) {
  return GetEntry(key).SetRaw(value);
}

bool SmartDashboard::SetDefaultRaw(std::string_view key,
                                   std::span<const uint8_t> defaultValue) {
  return GetEntry(key).SetDefaultRaw(defaultValue);
}

std::vector<uint8_t> SmartDashboard::GetRaw(
    std::string_view key, std::span<const uint8_t> defaultValue) {
  return GetEntry(key).GetRaw(defaultValue);
}

bool SmartDashboard::PutValue(std::string_view keyName,
                              const nt::Value& value) {
  return GetInstance().table->GetEntry(keyName).SetValue(value);
}

bool SmartDashboard::SetDefaultValue(std::string_view key,
                                     const nt::Value& defaultValue) {
  return GetEntry(key).SetDefaultValue(defaultValue);
}

nt::Value SmartDashboard::GetValue(std::string_view keyName) {
  return GetInstance().table->GetEntry(keyName).GetValue();
}

void SmartDashboard::PostListenerTask(std::function<void()> task) {
  GetInstance().listenerExecutor.Execute(task);
}

void SmartDashboard::UpdateValues() {
  auto& inst = GetInstance();
  inst.listenerExecutor.RunListenerTasks();
  std::scoped_lock lock(inst.tablesToDataMutex);
  for (auto& i : inst.tablesToData) {
    wpi::SendableRegistry::Update(i.second);
  }
}
