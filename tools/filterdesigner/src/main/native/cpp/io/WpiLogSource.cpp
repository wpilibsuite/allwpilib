// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/WpiLogSource.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

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
    auto [it, inserted] = m_entryIndex.try_emplace(name);
    if (inserted) {
      it->second.type = std::string{start.type};
      m_entries.push_back({name, it->second.type, IsNumericType(start.type),
                           name + "  [" + it->second.type + "]"});
    }
    it->second.ids.push_back(start.entry);
  }
}

std::optional<Signal> WpiLogSource::LoadEntry(std::string_view name) const {
  auto it = m_entryIndex.find(std::string{name});
  if (it == m_entryIndex.end() || !IsNumericType(it->second.type)) {
    return std::nullopt;
  }
  const auto& [type, ids] = it->second;

  Signal sig;
  sig.name = std::string{name};
  for (const auto& record : *m_reader) {
    if (record.IsControl()) {
      continue;
    }
    if (std::find(ids.begin(), ids.end(), record.GetEntry()) == ids.end()) {
      continue;
    }
    double value = 0.0;
    bool ok = false;
    if (type == "double") {
      ok = record.GetDouble(&value);
    } else if (type == "float") {
      float f = 0.0f;
      ok = record.GetFloat(&f);
      value = f;
    } else if (type == "int64") {
      int64_t i = 0;
      ok = record.GetInteger(&i);
      value = static_cast<double>(i);
    } else if (type == "boolean") {
      bool b = false;
      ok = record.GetBoolean(&b);
      value = b ? 1.0 : 0.0;
    }
    if (!ok) {
      continue;
    }
    sig.timestamps.push_back(record.GetTimestamp() * 1e-6);
    sig.values.push_back(value);
  }
  sig.sampleRate = Signal::InferSampleRate(sig.timestamps);
  sig.uniform = Signal::IsUniform(sig.timestamps);
  return sig;
}

}  // namespace wpi::filterdesigner
