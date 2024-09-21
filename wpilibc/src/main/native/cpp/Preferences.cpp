// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Preferences.h"

#include <memory>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <hal/FRCUsageReporting.h>
#include <networktables/MultiSubscriber.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTableListener.h>
#include <networktables/StringTopic.h>
#include <wpi/json.h>

using namespace frc;

// The Preferences table name
static constexpr std::string_view kTableName{"Preferences"};
static constexpr std::string_view kSmartDashboardType = "RobotPreferences";
namespace {
struct Instance {
  Instance();

  std::shared_ptr<nt::NetworkTable> table{
      nt::NetworkTableInstance::GetDefault().GetTable(kTableName)};
  nt::StringPublisher typePublisher{table->GetStringTopic(".type").PublishEx(
      nt::StringTopic::kTypeString, {{"SmartDashboard", kSmartDashboardType}})};
  nt::MultiSubscriber tableSubscriber{nt::NetworkTableInstance::GetDefault(),
                                      {{fmt::format("{}/", table->GetPath())}}};
  nt::NetworkTableListener listener;
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

std::vector<std::string> Preferences::GetKeys() {
  return ::GetInstance().table->GetKeys();
}

std::string Preferences::GetString(std::string_view key,
                                   std::string_view defaultValue) {
  return ::GetInstance().table->GetEntry(key).GetString(defaultValue);
}

int Preferences::GetInt(std::string_view key, int defaultValue) {
  return ::GetInstance().table->GetEntry(key).GetInteger(defaultValue);
}

double Preferences::GetDouble(std::string_view key, double defaultValue) {
  return ::GetInstance().table->GetEntry(key).GetDouble(defaultValue);
}

float Preferences::GetFloat(std::string_view key, float defaultValue) {
  return ::GetInstance().table->GetEntry(key).GetFloat(defaultValue);
}

bool Preferences::GetBoolean(std::string_view key, bool defaultValue) {
  return ::GetInstance().table->GetEntry(key).GetBoolean(defaultValue);
}

int64_t Preferences::GetLong(std::string_view key, int64_t defaultValue) {
  return ::GetInstance().table->GetEntry(key).GetInteger(defaultValue);
}

void Preferences::SetString(std::string_view key, std::string_view value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetString(value);
  entry.SetPersistent();
}

void Preferences::InitString(std::string_view key, std::string_view value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultString(value);
  entry.SetPersistent();
}

void Preferences::SetInt(std::string_view key, int value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetInteger(value);
  entry.SetPersistent();
}

void Preferences::InitInt(std::string_view key, int value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultInteger(value);
  entry.SetPersistent();
}

void Preferences::SetDouble(std::string_view key, double value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

void Preferences::InitDouble(std::string_view key, double value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultDouble(value);
  entry.SetPersistent();
}

void Preferences::SetFloat(std::string_view key, float value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetFloat(value);
  entry.SetPersistent();
}

void Preferences::InitFloat(std::string_view key, float value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultFloat(value);
  entry.SetPersistent();
}

void Preferences::SetBoolean(std::string_view key, bool value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetBoolean(value);
  entry.SetPersistent();
}

void Preferences::InitBoolean(std::string_view key, bool value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultBoolean(value);
  entry.SetPersistent();
}

void Preferences::SetLong(std::string_view key, int64_t value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetInteger(value);
  entry.SetPersistent();
}

void Preferences::InitLong(std::string_view key, int64_t value) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.SetDefaultInteger(value);
  entry.SetPersistent();
}

bool Preferences::ContainsKey(std::string_view key) {
  return ::GetInstance().table->ContainsKey(key);
}

void Preferences::Remove(std::string_view key) {
  auto entry = ::GetInstance().table->GetEntry(key);
  entry.ClearPersistent();
  entry.Unpublish();
}

void Preferences::RemoveAll() {
  for (auto preference : GetKeys()) {
    if (preference != ".type") {
      Remove(preference);
    }
  }
}

Instance::Instance() {
  typePublisher.Set(kSmartDashboardType);
  listener = nt::NetworkTableListener::CreateListener(
      tableSubscriber, NT_EVENT_PUBLISH | NT_EVENT_IMMEDIATE,
      [typeTopic = typePublisher.GetTopic().GetHandle()](auto& event) {
        if (auto topicInfo = event.GetTopicInfo()) {
          if (topicInfo->topic != typeTopic) {
            nt::SetTopicPersistent(topicInfo->topic, true);
          }
        }
      });
  HAL_Report(HALUsageReporting::kResourceType_Preferences, 0);
}
