// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryRegistry.hpp"

#include <memory>
#include <print>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/telemetry/DiscardTelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/telemetry/detail/PathUtil.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/htrie_map.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi;
using namespace wpi::telemetry;

static void DefaultReportWarning(std::string_view path, std::string_view msg);

namespace {
struct EntryMetadata {
  bool keepDuplicates = false;
  util::StringMap<std::string> properties;
};

struct EntryMetadataSnapshot {
  bool keepDuplicates = false;
  std::vector<std::pair<std::string, std::string>> properties;
};

struct RemovedEntry {
  std::shared_ptr<TelemetryBackend> backend;
  std::string path;
};

struct Instance {
  util::mutex mutex;
  util::htrie_map<char, std::shared_ptr<TelemetryBackend>> backends;
  util::StringMap<std::shared_ptr<TelemetryBackend>> entryBackends;
  util::StringMap<TelemetryTable> tables;
  util::StringMap<EntryMetadata> entryMetadata;
  util::mutex warningMutex;
  std::function<void(std::string_view, std::string_view)> reportWarning{
      DefaultReportWarning};
};
}  // namespace

static Instance& GetInstance() {
  static Instance inst;
  return inst;
}

static std::shared_ptr<TelemetryBackend> GetMissingBackend() {
  static auto backend = std::make_shared<DiscardTelemetryBackend>();
  return backend;
}

static std::shared_ptr<TelemetryBackend> GetBackendForNormalizedPath(
    Instance& inst, std::string_view path) {
  for (;;) {
    auto backendIt = inst.backends.find(path);
    if (backendIt != inst.backends.end()) {
      return backendIt.value();
    }

    size_t slash = path.find_last_of('/');
    if (slash == std::string_view::npos || slash == 0) {
      break;
    }
    path = path.substr(0, slash);
  }

  auto rootBackendIt = inst.backends.find("/");
  if (rootBackendIt != inst.backends.end()) {
    return rootBackendIt.value();
  }
  auto defaultBackendIt = inst.backends.find("");
  if (defaultBackendIt != inst.backends.end()) {
    return defaultBackendIt.value();
  }
  return nullptr;
}

static void DefaultReportWarning(std::string_view path, std::string_view msg) {
  // TODO: do something smarter here
  std::print(stderr, "Telemetry '{}': warning: {}\n", path, msg);
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
  std::function<void(std::string_view, std::string_view)> reportWarning;
  {
    std::scoped_lock lock{inst.warningMutex};
    reportWarning = inst.reportWarning;
  }
  reportWarning(path, msg);
}

void TelemetryRegistry::RegisterBackend(
    std::string_view prefix, std::shared_ptr<TelemetryBackend> backend) {
  std::string prefixBuf;
  prefix = detail::NormalizeBackendPrefix(prefix, prefixBuf);
  Instance& inst = GetInstance();
  std::vector<RemovedEntry> removedEntries;
  {
    std::scoped_lock lock{inst.mutex};
    // Reset table generations before backend routing changes become visible.
    for (auto& table : inst.tables) {
      table.second.Reset();
    }

    inst.backends[prefix] = std::move(backend);

    for (auto it = inst.entryBackends.begin();
         it != inst.entryBackends.end();) {
      std::shared_ptr<TelemetryBackend> newBackend =
          GetBackendForNormalizedPath(inst, it->first);
      if (it->second != newBackend) {
        removedEntries.push_back({it->second, it->first});
        it = inst.entryBackends.erase(it);
      } else {
        ++it;
      }
    }
  }

  for (auto& entry : removedEntries) {
    entry.backend->RemoveEntry(entry.path);
  }
}

std::shared_ptr<TelemetryBackend> TelemetryRegistry::GetBackend(
    std::string_view path) {
  std::string buf;
  path = detail::NormalizeName(path, buf);
  Instance& inst = GetInstance();
  {
    std::scoped_lock lock{inst.mutex};
    auto backend = GetBackendForNormalizedPath(inst, path);
    if (backend) {
      return backend;
    }
  }
  ReportWarning(path, "no backend for path");
  return GetMissingBackend();
}

TelemetryRegistry::EntryHandle TelemetryRegistry::GetEntryHandle(
    std::string_view path) {
  std::string buf;
  path = detail::NormalizeName(path, buf);
  Instance& inst = GetInstance();
  for (;;) {
    std::shared_ptr<TelemetryBackend> backend;
    bool missingBackend = false;
    {
      std::scoped_lock lock{inst.mutex};
      backend = GetBackendForNormalizedPath(inst, path);
      if (!backend) {
        backend = GetMissingBackend();
        missingBackend = true;
      }
    }
    if (missingBackend) {
      ReportWarning(path, "no backend for path");
    }

    std::shared_ptr<TelemetryEntry> entry = backend->GetEntry(path);
    {
      std::scoped_lock lock{inst.mutex};
      std::shared_ptr<TelemetryBackend> currentBackend =
          GetBackendForNormalizedPath(inst, path);
      if (!currentBackend) {
        currentBackend = GetMissingBackend();
      }
      if (currentBackend == backend) {
        inst.entryBackends[path] = backend;
        return {std::move(entry), std::move(backend)};
      }
    }

    backend->RemoveEntry(path);
  }
}

bool TelemetryRegistry::HasNonDiscardDescendant(std::string_view tablePath) {
  std::string buf;
  tablePath = detail::NormalizeTableName(tablePath, buf);
  std::vector<std::pair<std::string, std::shared_ptr<TelemetryBackend>>>
      descendants;
  {
    Instance& inst = GetInstance();
    std::scoped_lock lock{inst.mutex};
    for (auto it = inst.backends.begin(); it != inst.backends.end(); ++it) {
      std::string prefix;
      it.key(prefix);
      if (detail::IsPathOrDescendant(prefix, tablePath)) {
        descendants.emplace_back(std::move(prefix), it.value());
      }
    }
  }

  for (const auto& [prefix, backend] : descendants) {
    if (!backend->GetEntry(prefix)->IsDiscard()) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<TelemetryEntry> TelemetryRegistry::GetEntry(
    std::string_view path) {
  struct EntryOwner {
    std::shared_ptr<TelemetryEntry> entry;
    std::shared_ptr<TelemetryBackend> backend;
  };

  auto handle = GetEntryHandle(path);
  TelemetryEntry* entry = handle.entry.get();
  auto owner = std::make_shared<EntryOwner>(
      EntryOwner{std::move(handle.entry), std::move(handle.backend)});
  return {std::move(owner), entry};
}

void TelemetryRegistry::RecordKeepDuplicates(std::string_view path) {
  std::string buf;
  path = detail::NormalizeName(path, buf);
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  inst.entryMetadata[path].keepDuplicates = true;
}

void TelemetryRegistry::RecordProperty(std::string_view path,
                                       std::string_view key,
                                       std::string_view value) {
  std::string buf;
  path = detail::NormalizeName(path, buf);
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  inst.entryMetadata[path].properties[key] = value;
}

void TelemetryRegistry::ApplyEntryMetadata(std::string_view path,
                                           TelemetryEntry& entry) {
  EntryMetadataSnapshot snapshot;
  {
    std::string buf;
    path = detail::NormalizeName(path, buf);
    Instance& inst = GetInstance();
    std::scoped_lock lock{inst.mutex};
    auto metadataIt = inst.entryMetadata.find(path);
    if (metadataIt == inst.entryMetadata.end()) {
      return;
    }

    snapshot.keepDuplicates = metadataIt->second.keepDuplicates;
    snapshot.properties.reserve(metadataIt->second.properties.size());
    for (const auto& property : metadataIt->second.properties) {
      snapshot.properties.emplace_back(property.first, property.second);
    }
  }

  if (snapshot.keepDuplicates) {
    entry.KeepDuplicates();
  }
  for (const auto& [key, value] : snapshot.properties) {
    entry.SetProperty(key, value);
  }
}

TelemetryTable& TelemetryRegistry::GetTable(std::string_view path) {
  std::string buf;
  path = detail::NormalizeTableName(path, buf);
  Instance& inst = GetInstance();
  std::scoped_lock lock{inst.mutex};
  return inst.tables.try_emplace(path, path, TelemetryTable::private_init{})
      .first->second;
}

void TelemetryRegistry::Reset() {
  Instance& inst = GetInstance();
  std::vector<RemovedEntry> removedEntries;
  {
    std::scoped_lock lock{inst.mutex};

    // Reset table generations before backend routing changes become visible.
    for (auto& table : inst.tables) {
      table.second.Reset();
    }

    for (const auto& [path, backend] : inst.entryBackends) {
      removedEntries.push_back({backend, path});
    }

    // clear backends
    inst.backends.clear();
    inst.entryBackends.clear();
    inst.entryMetadata.clear();
  }

  for (auto& entry : removedEntries) {
    entry.backend->RemoveEntry(entry.path);
  }
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
