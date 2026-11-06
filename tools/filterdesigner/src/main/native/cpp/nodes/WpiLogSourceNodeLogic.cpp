// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/WpiLogSourceNodeLogic.hpp"

#include <algorithm>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wpi::filterdesigner {

namespace {

/** Restricts @p range to @p bounds, or reports that it selects nothing. */
std::optional<TimeRange> Clamp(TimeRange range, TimeRange bounds) {
  range.start = std::max(range.start, bounds.start);
  range.end = std::min(range.end, bounds.end);
  if (range.Empty()) {
    return std::nullopt;
  }
  return range;
}

}  // namespace

void WpiLogSourceNodeLogic::SetSource(std::optional<WpiLogSource> source) {
  m_source = std::move(source);
  // The tree is a view of the entry list, so it is rebuilt with the list and
  // never separately — a stale tree would offer entries this log has not got.
  std::vector<NameTreeItem> items;
  if (m_source) {
    items.reserve(m_source->Entries().size());
    for (const auto& entry : m_source->Entries()) {
      items.push_back({entry.name, entry.type, entry.numeric});
    }
  }
  m_entryTree = BuildNameTree(items);
  ClearSelection();
}

void WpiLogSourceNodeLogic::Reset() {
  SetSource(std::nullopt);
  m_logPath.clear();
  m_loadError.clear();
}

bool WpiLogSourceNodeLogic::OpenFile(std::string_view path) {
  auto src = WpiLogSource::FromFile(path);
  if (!src) {
    SetSource(std::nullopt);
    m_logPath = std::string{path};
    m_loadError = "Failed to open: " + std::string{path};
    return false;
  }
  SetSource(std::move(src));
  m_logPath = std::string{path};
  m_loadError.clear();
  return true;
}

bool WpiLogSourceNodeLogic::OpenBuffer(std::span<const uint8_t> buffer) {
  auto src = WpiLogSource::FromBuffer(buffer);
  if (!src) {
    SetSource(std::nullopt);
    m_logPath.clear();
    m_loadError = "Failed to open buffer";
    return false;
  }
  SetSource(std::move(src));
  m_logPath.clear();
  m_loadError.clear();
  return true;
}

void WpiLogSourceNodeLogic::RestoreFromPath(std::string_view path,
                                            std::string_view selectedEntry) {
  m_logPath = std::string{path};
  if (path.empty()) {
    SetSource(std::nullopt);
    m_loadError.clear();
    return;
  }
  auto src = WpiLogSource::FromFile(path);
  if (!src) {
    SetSource(std::nullopt);
    m_loadError =
        "Saved log not found: " + std::string{path} + " — re-pick the file.";
    return;
  }
  SetSource(std::move(src));
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
  // Raw, because every window is cut from the samples as logged. This is also
  // the only scan of the log an entry costs — dragging the timeline
  // afterwards re-cuts this copy.
  auto sig = m_source->LoadEntryRaw(name);
  if (!sig) {
    m_loadError = "Failed to load entry: " + std::string{name};
    return false;
  }
  m_selectedName = std::string{name};
  m_rawSignal = std::move(*sig);
  m_segments =
      Signal::FindSegments(m_rawSignal->timestamps,
                           Signal::InferSampleRate(m_rawSignal->timestamps));
  // The whole record: narrowing drops real samples, so it stays a button
  // rather than something picking an entry does on its own.
  m_range = FullRange();
  Republish();
  m_loadError.clear();
  return true;
}

bool WpiLogSourceNodeLogic::SelectRange(TimeRange range) {
  if (!m_rawSignal) {
    return false;
  }
  auto clamped = Clamp(range, FullRange());
  if (!clamped) {
    return false;
  }
  m_range = *clamped;
  Republish();
  return true;
}

void WpiLogSourceNodeLogic::SelectFullRange() {
  if (!m_rawSignal) {
    return;
  }
  m_range = FullRange();
  Republish();
}

bool WpiLogSourceNodeLogic::SelectLongestSegment() {
  if (m_segments.empty()) {
    return false;
  }
  // max_element keeps the first of equal spans, which is the earliest.
  auto longest = std::max_element(m_segments.begin(), m_segments.end(),
                                  [](const Segment& a, const Segment& b) {
                                    return a.Duration() < b.Duration();
                                  });
  m_range = longest->Range();
  Republish();
  return true;
}

TimeRange WpiLogSourceNodeLogic::FullRange() const {
  if (!m_rawSignal || m_rawSignal->timestamps.empty()) {
    return TimeRange{};
  }
  return TimeRange{m_rawSignal->timestamps.front(),
                   m_rawSignal->timestamps.back()};
}

void WpiLogSourceNodeLogic::Republish() {
  auto windowed = m_rawSignal->Window(m_range);
  windowed.revision = ++m_revisionCounter;
  m_selectedSignal = std::move(windowed);
}

void WpiLogSourceNodeLogic::ClearSelection() {
  m_selectedName.clear();
  m_rawSignal.reset();
  m_segments.clear();
  m_range = TimeRange{};
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
