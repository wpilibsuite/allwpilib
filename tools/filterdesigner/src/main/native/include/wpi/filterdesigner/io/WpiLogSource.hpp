// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
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
   * Loads the timeseries for an entry, resampled onto its inferred uniform
   * grid. Returns std::nullopt if no entry by that name exists or the entry
   * is non-numeric.
   */
  std::optional<Signal> LoadEntry(std::string_view name) const;

  /**
   * Loads an entry's samples exactly as logged — jittered timestamps, holes
   * and all — in time order, whatever order the file holds them in. This is
   * the form a time-range selection has to window, since
   * @ref Signal::ResampleToGrid measures whatever it is handed. Separate from
   * a re-load because a scan costs O(records) whichever entry it is after.
   */
  std::optional<Signal> LoadEntryRaw(std::string_view name) const;

 private:
  struct EntryInfo {
    /**
     * The type the picker labels the name with and LoadEntryRaw judges it
     * by: the first numeric type any of the name's lifetimes announced, or
     * the first type of all when none of them is numeric. A name is free to
     * be finished and restarted under another type, so the first
     * announcement alone does not say what the name holds.
     */
    std::string type;
    bool numeric = false;
    /** Index into m_entries, so a later lifetime can revise the label. */
    std::size_t entryIndex = 0;
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
