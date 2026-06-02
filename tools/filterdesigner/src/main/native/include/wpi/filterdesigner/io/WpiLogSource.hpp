// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"

namespace wpi::filterdesigner {

struct LogEntry {
  std::string name;
  std::string type;
  bool numeric;
  /** Pre-formatted "name  [type]" — cached so the UI doesn't rebuild it every
   * frame when rendering the entries list. */
  std::string label;
};

/**
 * Reads a WPILOG file and exposes its numeric entries as Signals.
 *
 * "Numeric" means one of "double", "float", "int64", or "boolean". Arrays
 * and strings are listed but not loadable — filter design only applies to
 * scalar timeseries.
 */
class WpiLogSource {
 public:
  /**
   * Wraps an in-memory buffer without copying. The caller must keep @p buffer
   * alive for the lifetime of this source.
   */
  static std::optional<WpiLogSource> FromBuffer(
      std::span<const uint8_t> buffer);

  /** Reads a .wpilog file into memory and opens it. */
  static std::optional<WpiLogSource> FromFile(std::string_view path);

  WpiLogSource(WpiLogSource&&) = default;
  WpiLogSource& operator=(WpiLogSource&&) = default;

  /** All entries in the log (numeric and non-numeric). */
  const std::vector<LogEntry>& Entries() const { return m_entries; }

  /**
   * Loads the timeseries for an entry. Returns std::nullopt if no entry by
   * that name exists or the entry is non-numeric.
   */
  std::optional<Signal> LoadEntry(std::string_view name) const;

 private:
  struct EntryInfo {
    std::string type;
    // A single name can span multiple IDs if the log was Finish'd and
    // restarted. Kept tiny (usually 1) so linear search is fine.
    std::vector<int> ids;
  };

  explicit WpiLogSource(std::unique_ptr<wpi::log::DataLogReader> reader);
  void ScanEntries();

  std::unique_ptr<wpi::log::DataLogReader> m_reader;
  std::vector<LogEntry> m_entries;
  std::unordered_map<std::string, EntryInfo> m_entryIndex;
};

/** True for types that LoadEntry can coerce to double. */
bool IsNumericType(std::string_view type);

}  // namespace wpi::filterdesigner
