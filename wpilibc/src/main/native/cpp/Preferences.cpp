// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Preferences.h"

#include <algorithm>

#include <hal/FRCUsageReporting.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>

using namespace frc;

// The Preferences table name
static constexpr std::string_view kTableName{"Preferences"};

namespace {
struct Instance {
  Instance();

  std::shared_ptr<nt::NetworkTable> table{
      nt::NetworkTableInstance::GetDefault().GetTable(kTableName)};
  NT_EntryListener listener;
};
}  // namespace

static Instance& GetInstance() {
  static Instance instance;
  return instance;
}

#ifndef __FRC_ROBORIO__
namespace frc::impl {
void ResetPreferencesInstance() {
  GetInstance() = Instance();
}
}  // namespace frc::impl
#endif

Preferences* Preferences::GetInstance() {
  ::GetInstance();
  static Preferences instance;
  return &instance;
}

std::vector<std::string> Preferences::GetKeys() {
  return ::GetInstance().table->GetKeys();
}

std::string Preferences::GetString(std::string_view key,
                                   std::string_view defaultValue) {
  return ::GetInstance().table->GetString(key, defaultValue);
}

int Preferences::GetInt(std::string_view key, int defaultValue) {
  return static_cast<int>(::GetInstance().table->GetNumber(key, defaultValue));
}

double Preferences::GetDouble(std::string_view key, double defaultValue) {
  return ::GetInstance().table->GetNumber(key, defaultValue);
}

float Preferences::GetFloat(std::string_view key, float defaultValue) {
  return ::GetInstance().table->GetNumber(key, defaultValue);
}

bool Preferences::GetBoolean(std::string_view key, bool defaultValue) {
  return ::GetInstance().table->GetBoolean(key, defaultValue);
}

int64_t Preferences::GetLong(std::string_view key, int64_t defaultValue) {
  return static_cast<int64_t>(
      ::GetInstance().table->GetNumber(key, defaultValue));
}

void Preferences::SetString(std::string_view key, std::string_view value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetString(value);
  entry.SetPersistent();
}

void Preferences::PutString(std::string_view key, std::string_view value) {
  SetString(key, value);
}

void Preferences::InitString(std::string_view key, std::string_view value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultString(value);
}

void Preferences::SetInt(std::string_view key, int value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

void Preferences::PutInt(std::string_view key, int value) {
  SetInt(key, value);
}

void Preferences::InitInt(std::string_view key, int value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultDouble(value);
}

void Preferences::SetDouble(std::string_view key, double value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

void Preferences::PutDouble(std::string_view key, double value) {
  SetDouble(key, value);
}

void Preferences::InitDouble(std::string_view key, double value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultDouble(value);
}

void Preferences::SetFloat(std::string_view key, float value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

void Preferences::PutFloat(std::string_view key, float value) {
  SetFloat(key, value);
}

void Preferences::InitFloat(std::string_view key, float value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultDouble(value);
}

void Preferences::SetBoolean(std::string_view key, bool value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetBoolean(value);
  entry.SetPersistent();
}

void Preferences::PutBoolean(std::string_view key, bool value) {
  SetBoolean(key, value);
}

void Preferences::InitBoolean(std::string_view key, bool value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultBoolean(value);
}

void Preferences::SetLong(std::string_view key, int64_t value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

void Preferences::PutLong(std::string_view key, int64_t value) {
  SetLong(key, value);
}

void Preferences::InitLong(std::string_view key, int64_t value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultDouble(value);
}

bool Preferences::ContainsKey(std::string_view key) {
  return ::GetInstance().table->ContainsKey(key);
}

void Preferences::Remove(std::string_view key) {
  ::GetInstance().table->Delete(key);
}

void Preferences::RemoveAll() {
  for (auto preference : GetKeys()) {
    if (preference != ".type") {
      Remove(preference);
    }
  }
}

Instance::Instance() {
  table->GetEntry(".type").SetString("RobotPreferences");
  listener = table->AddEntryListener(
      [=](nt::NetworkTable* table, std::string_view name,
          nt::NetworkTableEntry entry, std::shared_ptr<nt::Value> value,
          int flags) { entry.SetPersistent(); },
      NT_NOTIFY_NEW | NT_NOTIFY_IMMEDIATE);
  HAL_Report(HALUsageReporting::kResourceType_Preferences, 0);
}
