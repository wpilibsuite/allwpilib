/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Preferences.h"

#include <algorithm>

#include <hal/HAL.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/StringRef.h>

#include "frc/WPIErrors.h"

using namespace frc;

// The Preferences table name
static wpi::StringRef kTableName{"Preferences"};

Preferences* Preferences::GetInstance() {
  static Preferences instance;
  return &instance;
}

std::vector<std::string> Preferences::GetKeys() { return m_table->GetKeys(); }

std::string Preferences::GetString(wpi::StringRef key,
                                   wpi::StringRef defaultValue) {
  return m_table->GetString(key, defaultValue);
}

int Preferences::GetInt(wpi::StringRef key, int defaultValue) {
  return static_cast<int>(m_table->GetNumber(key, defaultValue));
}

double Preferences::GetDouble(wpi::StringRef key, double defaultValue) {
  return m_table->GetNumber(key, defaultValue);
}

float Preferences::GetFloat(wpi::StringRef key, float defaultValue) {
  return m_table->GetNumber(key, defaultValue);
}

bool Preferences::GetBoolean(wpi::StringRef key, bool defaultValue) {
  return m_table->GetBoolean(key, defaultValue);
}

int64_t Preferences::GetLong(wpi::StringRef key, int64_t defaultValue) {
  return static_cast<int64_t>(m_table->GetNumber(key, defaultValue));
}

void Preferences::PutString(wpi::StringRef key, wpi::StringRef value) {
  auto entry = m_table->GetEntry(key);
  entry.SetString(value);
  entry.SetPersistent();
}

void Preferences::PutInt(wpi::StringRef key, int value) {
  auto entry = m_table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

void Preferences::PutDouble(wpi::StringRef key, double value) {
  auto entry = m_table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

void Preferences::PutFloat(wpi::StringRef key, float value) {
  auto entry = m_table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

void Preferences::PutBoolean(wpi::StringRef key, bool value) {
  auto entry = m_table->GetEntry(key);
  entry.SetBoolean(value);
  entry.SetPersistent();
}

void Preferences::PutLong(wpi::StringRef key, int64_t value) {
  auto entry = m_table->GetEntry(key);
  entry.SetDouble(value);
  entry.SetPersistent();
}

bool Preferences::ContainsKey(wpi::StringRef key) {
  return m_table->ContainsKey(key);
}

void Preferences::Remove(wpi::StringRef key) { m_table->Delete(key); }

void Preferences::RemoveAll() {
  for (auto preference : GetKeys()) {
    if (preference != ".type") {
      Remove(preference);
    }
  }
}

Preferences::Preferences()
    : m_table(nt::NetworkTableInstance::GetDefault().GetTable(kTableName)) {
  m_table->GetEntry(".type").SetString("RobotPreferences");
  m_listener = m_table->AddEntryListener(
      [=](nt::NetworkTable* table, wpi::StringRef name,
          nt::NetworkTableEntry entry, std::shared_ptr<nt::Value> value,
          int flags) { entry.SetPersistent(); },
      NT_NOTIFY_NEW | NT_NOTIFY_IMMEDIATE);
  HAL_Report(HALUsageReporting::kResourceType_Preferences, 0);
}
