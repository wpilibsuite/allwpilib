// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/WpiLogSource.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <numeric>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/util/MemoryBuffer.hpp"

namespace wpi::filterdesigner {

bool IsNumericType(std::string_view type) {
  return type == "double" || type == "float" || type == "int64" ||
         type == "boolean";
}

std::optional<WpiLogSource> WpiLogSource::FromBuffer(
    std::span<const uint8_t> buffer) {
  auto mem = wpi::util::MemoryBuffer::GetMemBuffer(buffer, "wpilog");
  auto reader = std::make_unique<wpi::log::DataLogReader>(std::move(mem));
  if (!reader->IsValid()) {
    return std::nullopt;
  }
  return WpiLogSource{std::move(reader)};
}

std::optional<WpiLogSource> WpiLogSource::FromFile(std::string_view path) {
  auto fileBuffer = wpi::util::MemoryBuffer::GetFile(path);
  if (!fileBuffer) {
    return std::nullopt;
  }
  auto reader =
      std::make_unique<wpi::log::DataLogReader>(std::move(*fileBuffer));
  if (!reader->IsValid()) {
    return std::nullopt;
  }
  return WpiLogSource{std::move(reader)};
}

WpiLogSource::WpiLogSource(std::unique_ptr<wpi::log::DataLogReader> reader)
    : m_reader{std::move(reader)} {
  ScanEntries();
}

void WpiLogSource::ScanEntries() {
  for (const auto& record : *m_reader) {
    if (!record.IsStart()) {
      continue;
    }
    wpi::log::StartRecordData start;
    if (!record.GetStartData(&start)) {
      continue;
    }
    std::string name{start.name};
    const bool numeric = IsNumericType(start.type);
    auto [it, inserted] = m_entryIndex.try_emplace(name);
    if (inserted) {
      it->second.type = std::string{start.type};
      it->second.numeric = numeric;
      it->second.entryIndex = m_entries.size();
      m_entries.push_back({name, it->second.type, numeric});
      continue;
    }
    // A name whose first lifetime was a string but which comes back later as
    // a double does hold a loadable timeseries, and freezing the first
    // announcement would grey it out in the picker for good. The first
    // numeric lifetime is the one worth naming: labelling the row with a
    // type it will not load reads as an error in the file.
    if (numeric && !it->second.numeric) {
      it->second.type = std::string{start.type};
      it->second.numeric = true;
      m_entries[it->second.entryIndex].type = it->second.type;
      m_entries[it->second.entryIndex].numeric = true;
    }
  }
}

std::optional<Signal> WpiLogSource::LoadEntry(std::string_view name) const {
  auto sig = LoadEntryRaw(name);
  if (!sig) {
    return std::nullopt;
  }
  sig->ResampleToGrid();
  return sig;
}

std::optional<Signal> WpiLogSource::LoadEntryRaw(std::string_view name) const {
  auto it = m_entryIndex.find(std::string{name});
  if (it == m_entryIndex.end() || !it->second.numeric) {
    return std::nullopt;
  }
  const std::string& type = it->second.type;

  Signal sig;
  sig.name = std::string{name};
  // An entry id belongs to a name only between its Start and Finish records;
  // a Finish'd id may be handed to a different name later in the file. So
  // rather than matching on every id the name ever held, walk the control
  // records alongside the data and keep the lifetimes live for this name.
  // Usually one, so linear search is fine.
  //
  // Each carries the type its own Start announced: once an entry has been
  // finished, the writer lets the same name start again under a different
  // type (DataLog::StartImpl only rejects a mismatch while the entry is
  // still referenced), and decoding a later int64 lifetime as the double the
  // name opened with would bit-cast the payload into nonsense.
  struct Lifetime {
    int id;
    std::string type;
  };
  std::vector<Lifetime> live;
  bool sawSample = false;
  bool allBoolean = true;
  for (const auto& record : *m_reader) {
    if (record.IsStart()) {
      wpi::log::StartRecordData start;
      if (!record.GetStartData(&start)) {
        continue;
      }
      // Whoever the id was live for before, it belongs to this name now.
      std::erase_if(live,
                    [&](const Lifetime& l) { return l.id == start.entry; });
      if (start.name == name) {
        live.push_back({start.entry, std::string{start.type}});
      }
      continue;
    }
    if (record.IsFinish()) {
      int finished = 0;
      if (record.GetFinishEntry(&finished)) {
        std::erase_if(live,
                      [&](const Lifetime& l) { return l.id == finished; });
      }
      continue;
    }
    if (record.IsControl()) {
      continue;
    }
    const auto lifetime =
        std::ranges::find(live, record.GetEntry(), &Lifetime::id);
    if (lifetime == live.end()) {
      continue;
    }
    const std::string& recordType = lifetime->type;
    double value = 0.0;
    bool ok = false;
    if (recordType == "double") {
      ok = record.GetDouble(&value);
    } else if (recordType == "float") {
      float f = 0.0f;
      ok = record.GetFloat(&f);
      value = f;
    } else if (recordType == "int64") {
      int64_t i = 0;
      ok = record.GetInteger(&i);
      value = static_cast<double>(i);
    } else if (recordType == "boolean") {
      bool b = false;
      ok = record.GetBoolean(&b);
      value = b ? 1.0 : 0.0;
    }
    if (!ok) {
      continue;
    }
    sawSample = true;
    if (recordType != "boolean") {
      allBoolean = false;
    }
    sig.timestamps.push_back(record.GetTimestamp() * 1e-9);
    sig.values.push_back(value);
  }
  // Records come back in file order, and a writer is free to hand the log
  // timestamps that run backwards. Everything downstream — the resampler,
  // the segment finder, the window's binary search — assumes time order, so
  // restore it here. Stable, so samples sharing a timestamp keep their
  // logged order.
  if (!std::ranges::is_sorted(sig.timestamps)) {
    std::vector<std::size_t> order(sig.timestamps.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::ranges::stable_sort(order, {},
                             [&](std::size_t i) { return sig.timestamps[i]; });
    std::vector<double> timestamps;
    std::vector<double> values;
    timestamps.reserve(order.size());
    values.reserve(order.size());
    for (std::size_t i : order) {
      timestamps.push_back(sig.timestamps[i]);
      values.push_back(sig.values[i]);
    }
    sig.timestamps = std::move(timestamps);
    sig.values = std::move(values);
  }
  // Integers stay continuous: an int64 entry is more often a count than a
  // state enum. A name whose lifetimes disagree only holds still if every
  // one of them that produced a sample was boolean.
  sig.discrete = sawSample ? allBoolean : type == "boolean";
  return sig;
}

}  // namespace wpi::filterdesigner
