// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryRegistry.h"

#include <memory>
#include <stdexcept>

#include <fmt/format.h>

#include "wpi/htrie_map.h"
#include "wpi/mutex.h"
#include "wpi/telemetry/TelemetryBackend.h"
#include "wpi/telemetry/TelemetryTable.h"
#include "wpi/StringMap.h"

using namespace wpi;

namespace {
struct Instance {
  wpi::mutex mutex;
  wpi::htrie_map<char, std::shared_ptr<TelemetryBackend>> backends;
  wpi::StringMap<TelemetryTable> tables;
};
}  // namespace

static Instance& GetInstance() {
  static Instance inst;
  return inst;
}

void TelemetryRegistry::RegisterBackend(
    std::string_view prefix, std::shared_ptr<TelemetryBackend> backend) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  inst.backends.emplace(prefix, std::move(backend));
}

std::shared_ptr<TelemetryBackend> TelemetryRegistry::GetBackend(
    std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  auto backendIt = inst.backends.longest_prefix(path);
  if (backendIt == inst.backends.end()) {
    throw std::out_of_range(fmt::format("no backend for path '{}'", path));
  }
  return backendIt.value();
}

TelemetryEntry& TelemetryRegistry::GetEntry(std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  auto backendIt = inst.backends.longest_prefix(path);
  if (backendIt == inst.backends.end()) {
    throw std::out_of_range(fmt::format("no backend for path '{}'", path));
  }
  return backendIt.value()->GetEntry(path);
}

TelemetryTable& TelemetryRegistry::GetTable(std::string_view path) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  return inst.tables.try_emplace(path, path, TelemetryTable::private_init{})
      .first->second;
}

void TelemetryRegistry::Reset() {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};

  // reset cached entries in tables
  for (auto& table : inst.tables) {
    table.second.Reset();
  }

  // clear backends
  inst.backends.clear();
}

bool TelemetryRegistry::HasSchema(TelemetryBackend& backend,
                                  std::string_view schemaName) {
  return backend.HasSchema(schemaName);
}

void TelemetryRegistry::AddSchema(TelemetryBackend& backend,
                                  std::string_view schemaName,
                                  std::string_view type,
                                  std::span<const uint8_t> schema) {
  backend.AddSchema(schemaName, type, schema);
}

void TelemetryRegistry::AddSchema(TelemetryBackend& backend,
                                  std::string_view schemaName,
                                  std::string_view type,
                                  std::string_view schema) {
  backend.AddSchema(schemaName, type, schema);
}
