// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "wpi/filterdesigner/io/WpiLogSource.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"

namespace pfd {
class open_file;
}  // namespace pfd

namespace wpi::filterdesigner {

/**
 * ImGui view for loading a .wpilog file and picking a numeric entry.
 *
 * The view holds the most recently loaded entry as the "selected signal" —
 * downstream views (plots, pipeline) read it via @ref SelectedSignal and
 * may compare the pointer to detect selection changes.
 */
class WpiLogLoaderView {
 public:
  WpiLogLoaderView();
  ~WpiLogLoaderView();

  WpiLogLoaderView(const WpiLogLoaderView&) = delete;
  WpiLogLoaderView& operator=(const WpiLogLoaderView&) = delete;

  /** Draw the view. Must be inside an ImGui window. */
  void Display();

  /**
   * Latest selected entry's timeseries, or nullptr if nothing is loaded.
   * Pointer remains stable until the next selection change.
   */
  const Signal* SelectedSignal() const {
    return m_selected ? &*m_selected : nullptr;
  }

 private:
  void PollFileDialog();

  std::unique_ptr<pfd::open_file> m_opener;
  std::string m_loadError;
  std::string m_loadedPath;
  std::optional<WpiLogSource> m_source;
  std::optional<Signal> m_selected;
  // Bumped on every entry assignment so downstream caches see content
  // change even when the optional's address (and the new entry's sample
  // count) happen to coincide with the previous selection.
  std::uint64_t m_revisionCounter = 0;
};

}  // namespace wpi::filterdesigner
