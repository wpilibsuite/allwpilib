// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryTable.hpp"

#include <format>
#include <string>
#include <utility>

#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"

using namespace wpi;
using namespace wpi::telemetry;

std::string_view TelemetryTable::GetPath() const {
  return m_path;
}

bool TelemetryTable::SetType(std::string_view typeString) {
  for (;;) {
    std::string expectedType;
    uint64_t resetGeneration = 0;
    {
      std::scoped_lock lock{m_mutex};
      if (!m_type.empty()) {
        if (m_type == typeString) {
          return true;
        }
        expectedType = m_type;
      } else {
        m_type = typeString;
        resetGeneration = m_resetGeneration;
      }
    }
    if (!expectedType.empty()) {
      TypeMismatch(expectedType, typeString);
      return false;
    }

    auto entry = GetEntry(".type");
    {
      std::scoped_lock lock{m_mutex};
      if (m_resetGeneration != resetGeneration || m_type != typeString) {
        continue;
      }
    }
    if (!entry->IsDiscard()) {
      entry->LogString(typeString, "string");
    }
    return true;
  }
}

std::string TelemetryTable::GetType() const {
  std::scoped_lock lock{m_mutex};
  return m_type;
}

bool TelemetryTable::HasType() const {
  std::scoped_lock lock{m_mutex};
  return !m_type.empty();
}

TelemetryTable& TelemetryTable::GetTable(std::string_view name) const {
  std::unique_lock lock{m_mutex};
  auto aliasIt = m_tableAliasesMap.find(name);
  if (aliasIt != m_tableAliasesMap.end()) {
    return *aliasIt->second;
  }
  lock.unlock();  // avoid lock inversion
  TelemetryTable* newTable =
      &TelemetryRegistry::GetTable(std::format("{}{}/", m_path, name));
  std::string_view path = newTable->GetPath();
  lock.lock();
  auto tableIt = m_tablesMap.find(path);
  TelemetryTable* table = newTable;
  if (tableIt != m_tablesMap.end()) {
    table = tableIt->second;
  } else {
    table = m_tablesMap.try_emplace(path, table).first->second;
  }
  m_tableAliasesMap.try_emplace(name, table);
  return *table;
}

void TelemetryTable::KeepDuplicates(std::string_view name) {
  auto path = std::format("{}{}", m_path, name);
  for (;;) {
    uint64_t resetGeneration;
    {
      std::scoped_lock lock{m_mutex};
      resetGeneration = m_resetGeneration;
    }

    TelemetryRegistry::RecordKeepDuplicates(path);
    bool metadataApplied = false;
    auto entry = GetEntry(name, &metadataApplied);
    if (!metadataApplied && !entry->IsDiscard()) {
      entry->KeepDuplicates();
    }

    std::scoped_lock lock{m_mutex};
    if (m_resetGeneration == resetGeneration) {
      return;
    }
  }
}

void TelemetryTable::SetProperty(std::string_view name, std::string_view key,
                                 std::string_view value) {
  auto path = std::format("{}{}", m_path, name);
  for (;;) {
    uint64_t resetGeneration;
    {
      std::scoped_lock lock{m_mutex};
      resetGeneration = m_resetGeneration;
    }

    TelemetryRegistry::RecordProperty(path, key, value);
    bool metadataApplied = false;
    auto entry = GetEntry(name, &metadataApplied);
    if (!metadataApplied && !entry->IsDiscard()) {
      entry->SetProperty(key, value);
    }

    std::scoped_lock lock{m_mutex};
    if (m_resetGeneration == resetGeneration) {
      return;
    }
  }
}

void TelemetryTable::Log(std::string_view name, bool value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogBoolean(value);
}

void TelemetryTable::Log(std::string_view name, int8_t value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogInt8(value);
}

void TelemetryTable::Log(std::string_view name, int16_t value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogInt16(value);
}

void TelemetryTable::Log(std::string_view name, int32_t value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogInt32(value);
}

void TelemetryTable::Log(std::string_view name, int64_t value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogInt64(value);
}

void TelemetryTable::Log(std::string_view name, float value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogFloat(value);
}

void TelemetryTable::Log(std::string_view name, double value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogDouble(value);
}

void TelemetryTable::Log(std::string_view name, std::string_view value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogString(value, "string");
}

void TelemetryTable::Log(std::string_view name, std::string_view value,
                         std::string_view typeString) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogString(value, typeString);
}

void TelemetryTable::Log(std::string_view name, std::span<const bool> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogBooleanArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int16_t> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogInt16Array(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int32_t> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogInt32Array(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int64_t> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogInt64Array(value);
}

void TelemetryTable::Log(std::string_view name, std::span<const float> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogFloatArray(value);
}

void TelemetryTable::Log(std::string_view name, std::span<const double> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogDoubleArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const std::string> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogStringArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const std::string_view> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogStringArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const uint8_t> value) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogRaw(value, "raw");
}

void TelemetryTable::Log(std::string_view name, std::span<const uint8_t> value,
                         std::string_view typeString) {
  auto entry = GetEntry(name);
  if (entry->IsDiscard()) {
    return;
  }
  entry->LogRaw(value, typeString);
}

TelemetryTable::EntryHandle TelemetryTable::GetEntry(std::string_view name) {
  return GetEntry(name, nullptr);
}

TelemetryTable::EntryHandle TelemetryTable::GetEntry(std::string_view name,
                                                     bool* metadataApplied) {
  if (metadataApplied) {
    *metadataApplied = false;
  }

  for (;;) {
    std::unique_lock lock{m_mutex};
    auto entryIt = m_entriesMap.find(name);
    if (entryIt != m_entriesMap.end()) {
      auto entry = entryIt->second;
      lock.unlock();
      return entry;
    }
    uint64_t resetGeneration = m_resetGeneration;
    lock.unlock();  // avoid lock inversion

    auto path = std::format("{}{}", m_path, name);
    auto newEntry = TelemetryRegistry::GetEntryHandle(path);
    bool entryMetadataApplied = false;
    if (!newEntry.entry->IsDiscard()) {
      TelemetryRegistry::ApplyEntryMetadata(path, *newEntry.entry);
      entryMetadataApplied = true;
    }

    lock.lock();
    auto entryIt2 = m_entriesMap.find(name);
    if (entryIt2 != m_entriesMap.end()) {
      auto entry = entryIt2->second;
      lock.unlock();
      return entry;
    }
    if (resetGeneration != m_resetGeneration) {
      lock.unlock();
      continue;
    }
    EntryHandle entry{name, resetGeneration, std::move(newEntry.entry),
                      std::move(newEntry.backend)};
    auto insertedIt = m_entriesMap.try_emplace(name, entry).first;
    entry = insertedIt->second;
    lock.unlock();
    if (metadataApplied) {
      *metadataApplied = entryMetadataApplied;
    }
    return entry;
  }
}

bool TelemetryTable::ShouldLogTableValue(std::string_view name) {
  auto entry = GetEntry(name);
  if (!entry->IsDiscard()) {
    return true;
  }
  return GetTable(name).HasNonDiscardDescendant();
}

bool TelemetryTable::HasNonDiscardDescendant() {
  for (;;) {
    uint64_t resetGeneration;
    {
      std::scoped_lock lock{m_mutex};
      if (m_hasNonDiscardDescendantCached) {
        return m_hasNonDiscardDescendant;
      }
      resetGeneration = m_resetGeneration;
    }

    bool hasNonDiscardDescendant =
        TelemetryRegistry::HasNonDiscardDescendant(GetPath());
    std::scoped_lock lock{m_mutex};
    if (m_hasNonDiscardDescendantCached) {
      return m_hasNonDiscardDescendant;
    }
    if (resetGeneration != m_resetGeneration) {
      continue;
    }
    m_hasNonDiscardDescendant = hasNonDiscardDescendant;
    m_hasNonDiscardDescendantCached = true;
    return m_hasNonDiscardDescendant;
  }
}

void TelemetryTable::TypeMismatch(std::string_view expectedType,
                                  std::string_view typeString) {
  TelemetryRegistry::ReportWarning(
      m_path, std::format("table type mismatch, expected '{}', got '{}'",
                          expectedType, typeString));
}

void TelemetryTable::Reset() {
  std::scoped_lock lock{m_mutex};
  m_tableAliasesMap.clear();
  m_tablesMap.clear();
  m_entriesMap.clear();
  m_type.clear();
  m_hasNonDiscardDescendant = false;
  m_hasNonDiscardDescendantCached = false;
  ++m_resetGeneration;
}
