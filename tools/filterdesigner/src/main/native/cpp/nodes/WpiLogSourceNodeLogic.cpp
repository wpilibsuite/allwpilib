// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/WpiLogSourceNodeLogic.hpp"

#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace wpi::filterdesigner {

void WpiLogSourceNodeLogic::Reset() {
  m_source.reset();
  m_logPath.clear();
  m_selectedName.clear();
  m_selectedSignal.reset();
  m_loadError.clear();
}

bool WpiLogSourceNodeLogic::OpenFile(std::string_view path) {
  auto src = WpiLogSource::FromFile(path);
  if (!src) {
    m_source.reset();
    m_logPath = std::string{path};
    m_selectedName.clear();
    m_selectedSignal.reset();
    m_loadError = "Failed to open: " + std::string{path};
    return false;
  }
  m_source = std::move(src);
  m_logPath = std::string{path};
  m_selectedName.clear();
  m_selectedSignal.reset();
  m_loadError.clear();
  return true;
}

bool WpiLogSourceNodeLogic::OpenBuffer(std::span<const uint8_t> buffer) {
  auto src = WpiLogSource::FromBuffer(buffer);
  if (!src) {
    m_source.reset();
    m_logPath.clear();
    m_selectedName.clear();
    m_selectedSignal.reset();
    m_loadError = "Failed to open buffer";
    return false;
  }
  m_source = std::move(src);
  m_logPath.clear();
  m_selectedName.clear();
  m_selectedSignal.reset();
  m_loadError.clear();
  return true;
}

void WpiLogSourceNodeLogic::RestoreFromPath(std::string_view path,
                                            std::string_view selectedEntry) {
  m_logPath = std::string{path};
  m_selectedName.clear();
  m_selectedSignal.reset();
  if (path.empty()) {
    m_source.reset();
    m_loadError.clear();
    return;
  }
  auto src = WpiLogSource::FromFile(path);
  if (!src) {
    m_source.reset();
    m_loadError =
        "Saved log not found: " + std::string{path} + " — re-pick the file.";
    return;
  }
  m_source = std::move(src);
  m_loadError.clear();
  if (!selectedEntry.empty()) {
    // Best-effort: if the entry no longer exists in the log, leave the
    // selection empty and surface an error banner.
    if (!SelectEntry(selectedEntry)) {
      m_loadError =
          "Saved entry '" + std::string{selectedEntry} + "' not found in log";
    }
  }
}

bool WpiLogSourceNodeLogic::SelectEntry(std::string_view name) {
  if (!m_source) {
    m_loadError = "No log loaded";
    return false;
  }
  auto sig = m_source->LoadEntry(name);
  if (!sig) {
    m_loadError = "Failed to load entry: " + std::string{name};
    return false;
  }
  m_selectedName = std::string{name};
  sig->revision = ++m_revisionCounter;
  m_selectedSignal = std::move(*sig);
  m_loadError.clear();
  return true;
}

void WpiLogSourceNodeLogic::ClearSelection() {
  m_selectedName.clear();
  m_selectedSignal.reset();
}

std::span<const LogEntry> WpiLogSourceNodeLogic::Entries() const {
  if (!m_source) {
    return {};
  }
  const auto& entries = m_source->Entries();
  return std::span<const LogEntry>{entries.data(), entries.size()};
}

}  // namespace wpi::filterdesigner
