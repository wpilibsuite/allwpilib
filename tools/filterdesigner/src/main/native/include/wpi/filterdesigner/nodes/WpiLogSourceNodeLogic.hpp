// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/filterdesigner/io/WpiLogSource.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/nodes/NameTree.hpp"

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) state of a WpiLogSourceNode: a loaded log, the currently
 * picked entry, the time window selected within it, and the cached
 * @ref Signal that the node's output pin exposes. Designed to be unit-tested
 * without ImGui.
 *
 * The node UI calls @ref OpenFile when the user picks a .wpilog through the
 * file dialog, and @ref RestoreFromPath on graph load. Both share the
 * underlying open path, but the restore path is silent on failure (the node
 * renders an "needs re-pick" banner instead of throwing a dialog).
 *
 * **Raw samples and the published window.** A picked entry is held twice: as
 * @ref RawSignal, the samples exactly as logged, and as @ref Signal, the
 * selected window of those samples resampled onto its own uniform grid. Every
 * window is cut from the raw copy and resampled once — never from a previous
 * window's grid, which would report the slice as flawless however much of the
 * grid beneath it was interpolant. Holding the raw copy also keeps a marker
 * drag off the O(records) log scan @ref WpiLogSource::LoadEntryRaw costs.
 *
 * The window starts as the whole record. Narrowing usually cuts a lot of
 * interpolant, but it discards real samples too, which makes it the user's
 * call; @ref SelectLongestSegment is the one-click version.
 */
class WpiLogSourceNodeLogic {
 public:
  WpiLogSourceNodeLogic() = default;

  WpiLogSourceNodeLogic(const WpiLogSourceNodeLogic&) = delete;
  WpiLogSourceNodeLogic& operator=(const WpiLogSourceNodeLogic&) = delete;

  /**
   * Opens a .wpilog file from disk, clearing any previous selection. On
   * failure the logic resets and @ref LoadError carries a user-facing message.
   *
   * @return true on success.
   */
  bool OpenFile(std::string_view path);

  /**
   * Opens a .wpilog from an in-memory buffer. Path stays empty (used by
   * tests so they don't need to write a temp file).
   */
  bool OpenBuffer(std::span<const uint8_t> buffer);

  /**
   * @ref OpenFile for the deserialize path: a missing file leaves the logic
   * empty with an error rather than handing the caller a failure.
   */
  void RestoreFromPath(std::string_view path, std::string_view selectedEntry);

  /**
   * Picks a numeric entry by name. Returns true on success; on failure the
   * previous selection (if any) is kept and an error is exposed via
   * @ref LoadError.
   */
  bool SelectEntry(std::string_view name);

  /**
   * Narrows the published window to @p range, clamped to the record.
   *
   * The window is cut from the raw samples and resampled onto the grid it
   * implies, so a selection inside one segment gets a rate and a fill fraction
   * describing itself. The published @ref Signal is reseated and its revision
   * bumped, so a caller driving this from a drag should commit on release
   * rather than per frame.
   *
   * @return false, leaving the selection alone, when no entry is picked or
   *         when @p range clamps to nothing (inverted, or entirely outside
   *         the record).
   */
  bool SelectRange(TimeRange range);

  /** Widens the published window back to the whole record. No-op with no
   * entry picked. */
  void SelectFullRange();

  /**
   * Narrows the published window to the longest stretch of uninterrupted
   * logging, by wall-clock span; ties go to the earliest.
   *
   * @return false when the entry has no segments, i.e. when no sample rate
   *         could be inferred to measure gaps against.
   */
  bool SelectLongestSegment();

  /** Full time span of the picked entry's raw samples; a zero-width range at
   * the origin when nothing is picked. */
  TimeRange FullRange() const;

  /** The window currently published, within @ref FullRange. */
  const TimeRange& SelectedRange() const { return m_range; }

  /** Stretches of uninterrupted logging in the picked entry, in time order.
   * Empty when nothing is picked or no rate could be inferred. */
  std::span<const Segment> Segments() const { return m_segments; }

  /**
   * The picked entry's samples as logged, which the node's timeline draws;
   * nullptr when nothing is picked. Reseated by the same mutators as
   * @ref Signal, under the same borrow contract.
   */
  const wpi::filterdesigner::Signal* RawSignal() const {
    return m_rawSignal.has_value() ? &*m_rawSignal : nullptr;
  }

  /** Clears the loaded signal but keeps the open log + path. */
  void ClearSelection();

  /** True if a log is currently open. */
  bool HasFile() const { return m_source.has_value(); }

  /** Path the log was loaded from, or empty (buffer-loaded, or no load). */
  const std::string& LogPath() const { return m_logPath; }

  /** Name of the currently selected entry, or empty. */
  const std::string& SelectedEntry() const { return m_selectedName; }

  /** Last error message (empty when the logic is in a healthy state). */
  const std::string& LoadError() const { return m_loadError; }

  /**
   * Pointer to the loaded signal, or nullptr if nothing is selected.
   *
   * **Single-frame borrow contract.** The pointer is only valid until the
   * next mutator on this logic — every one of them reseats the underlying
   * @c std::optional<Signal> in place, destroying the old one. Sinks pulling
   * this through an ImNodeFlow OutPin must consume it within the frame and
   * must not cache it; `Signal::revision` is the cache key for "did the
   * underlying data change".
   */
  const wpi::filterdesigner::Signal* Signal() const {
    return m_selectedSignal.has_value() ? &*m_selectedSignal : nullptr;
  }

  /** All entries in the log (numeric + non-numeric). Empty when no log. */
  std::span<const LogEntry> Entries() const;

  /**
   * The same entries filed into a path-split tree for the node's picker, with
   * non-numeric entries marked not selectable. Rebuilt per opened log, not per
   * frame. The root is a container; render its children.
   */
  const NameTreeNode& EntryTree() const { return m_entryTree; }

  /** Drops everything — empty path, no log, no signal. */
  void Reset();

 private:
  /** Re-cuts @ref m_selectedSignal from @ref m_rawSignal at @ref m_range and
   * bumps its revision. */
  void Republish();

  /**
   * Installs @p source and rebuilds everything derived from it. The only place
   * @ref m_source is written, so a picker can never outlive its log.
   */
  void SetSource(std::optional<WpiLogSource> source);

  std::optional<WpiLogSource> m_source;
  NameTreeNode m_entryTree;
  std::string m_logPath;
  std::string m_selectedName;
  std::optional<wpi::filterdesigner::Signal> m_rawSignal;
  std::vector<Segment> m_segments;
  TimeRange m_range;
  std::optional<wpi::filterdesigner::Signal> m_selectedSignal;
  std::string m_loadError;
  std::uint64_t m_revisionCounter = 0;
};

}  // namespace wpi::filterdesigner
