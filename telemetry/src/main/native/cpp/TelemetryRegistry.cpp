// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryRegistry.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/htrie_map.h>
#include <wpi/mutex.h>

#include "wpi/telemetry/TelemetryBackend.h"
#include "wpi/telemetry/TelemetryTable.h"

using namespace wpi;

static void DefaultReportWarning(std::string_view path, std::string_view msg);

namespace {
struct Instance {
  wpi::mutex mutex;
  wpi::htrie_map<char, std::shared_ptr<TelemetryBackend>> backends;
  wpi::StringMap<TelemetryTable> tables;
  wpi::mutex warningMutex;
  std::function<void(std::string_view, std::string_view)> reportWarning{
      DefaultReportWarning};
};
}  // namespace

static Instance& GetInstance() {
  static Instance inst;
  return inst;
}

static std::string_view NormalizeName(std::string_view path, std::string& buf) {
  // common case is a well formatted name, so check first
  if (wpi::starts_with(path, '/') && !wpi::contains(path, "//")) {
    return path;
  }
  buf.clear();
  buf.reserve(path.size() + 2);
  if (!wpi::starts_with(path, '/')) {
    buf.push_back('/');
  }
  char prevCh = '\0';
  for (auto ch : path) {
    if (ch != '/' || prevCh != '/') {
      buf.push_back(ch);
    }
    prevCh = ch;
  }
  return buf;
}

static std::string_view NormalizeTableName(std::string_view path,
                                           std::string& buf) {
  // common case is a well formatted name, so check first
  if (wpi::starts_with(path, '/') && wpi::ends_with(path, '/') &&
      !wpi::contains(path, "//")) {
    return path;
  }
  NormalizeName(path, buf);
  if (!wpi::ends_with(path, '/')) {
    buf.push_back('/');
  }
  return buf;
}

static void DefaultReportWarning(std::string_view path, std::string_view msg) {
  // TODO: do something smarter here
  fmt::print(stderr, "Telemetry '{}': warning: {}\n", path, msg);
}

void TelemetryRegistry::SetReportWarning(
    std::function<void(std::string_view path, std::string_view msg)> func) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.warningMutex};
  if (func) {
    inst.reportWarning = std::move(func);
  } else {
    inst.reportWarning = DefaultReportWarning;
  }
}

std::function<void(std::string_view path, std::string_view msg)>
TelemetryRegistry::GetReportWarning() {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.warningMutex};
  return inst.reportWarning;
}

void TelemetryRegistry::ReportWarning(std::string_view path,
                                      std::string_view msg) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.warningMutex};
  inst.reportWarning(path, msg);
}

void TelemetryRegistry::RegisterBackend(
    std::string_view prefix, std::shared_ptr<TelemetryBackend> backend) {
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  inst.backends[prefix] = std::move(backend);

  // reset cached entries in tables
  for (auto& table : inst.tables) {
    table.second.Reset();
  }
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
  std::string buf;
  path = NormalizeName(path, buf);
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  auto backendIt = inst.backends.longest_prefix(path);
  if (backendIt == inst.backends.end()) {
    throw std::out_of_range(fmt::format("no backend for path '{}'", path));
  }
  return backendIt.value()->GetEntry(path);
}

TelemetryTable& TelemetryRegistry::GetTable(std::string_view path) {
  std::string buf;
  path = NormalizeTableName(path, buf);
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
