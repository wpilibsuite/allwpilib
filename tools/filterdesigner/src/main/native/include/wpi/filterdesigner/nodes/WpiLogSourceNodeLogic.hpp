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

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) state of a WpiLogSourceNode: a loaded log, the currently
 * picked entry, and the cached @ref Signal that the node's output pin
 * exposes. Designed to be unit-tested without ImGui.
 *
 * The node UI calls @ref OpenFile when the user picks a .wpilog through the
 * file dialog, and @ref RestoreFromPath on graph load. Both share the
 * underlying open path, but the restore path is silent on failure (the node
 * renders an "needs re-pick" banner instead of throwing a dialog).
 */
class WpiLogSourceNodeLogic {
 public:
  WpiLogSourceNodeLogic() = default;

  WpiLogSourceNodeLogic(const WpiLogSourceNodeLogic&) = delete;
  WpiLogSourceNodeLogic& operator=(const WpiLogSourceNodeLogic&) = delete;

  /**
   * Opens a .wpilog file from disk. On success the entry list becomes
   * available and any previous selection is cleared; on failure the logic
   * resets and @ref LoadError carries a user-facing message.
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
   * Same as @ref OpenFile but for the deserialize path — if the file is
   * missing the logic ends up in an empty + error state instead of bubbling
   * up the failure as something the caller has to handle.
   */
  void RestoreFromPath(std::string_view path, std::string_view selectedEntry);

  /**
   * Picks a numeric entry by name. Returns true on success; on failure the
   * previous selection (if any) is kept and an error is exposed via
   * @ref LoadError.
   */
  bool SelectEntry(std::string_view name);

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
   * next mutator on this logic — @ref OpenFile / @ref OpenBuffer /
   * @ref SelectEntry / @ref RestoreFromPath / @ref Reset all reseat the
   * underlying @c std::optional<Signal> in place, which destroys the old
   * @c Signal and frees its `values` / `timestamps` buffers. Downstream
   * sinks that pull this pointer through an ImNodeFlow OutPin must consume
   * it within the same frame and must not cache it across frames; the
   * `Signal::revision` field is the cache key intended for "did this
   * underlying data change" tracking. Sources that need cross-frame
   * pointer stability for downstream caching would need a heap-allocated
   * Signal owned by the logic instead.
   */
  const wpi::filterdesigner::Signal* Signal() const {
    return m_selectedSignal.has_value() ? &*m_selectedSignal : nullptr;
  }

  /** All entries in the log (numeric + non-numeric). Empty when no log. */
  std::span<const LogEntry> Entries() const;

  /** Drops everything — empty path, no log, no signal. */
  void Reset();

 private:
  std::optional<WpiLogSource> m_source;
  std::string m_logPath;
  std::string m_selectedName;
  std::optional<wpi::filterdesigner::Signal> m_selectedSignal;
  std::string m_loadError;
  std::uint64_t m_revisionCounter = 0;
};

}  // namespace wpi::filterdesigner
