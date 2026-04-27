// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "wpi/filterdesigner/model/Stage.hpp"

namespace pfd {
class open_file;
class save_file;
}  // namespace pfd

namespace wpi::filterdesigner {

/**
 * ImGui panel for editing a chain of filter stages and re-running the
 * corresponding design functions on every edit.
 *
 * The combined cascade is the concatenation of every stage's biquad sections,
 * which still composes into a single @c BiquadFilter at runtime. The result
 * is exposed via @ref Result so other views can render it.
 */
class FilterDesignView {
 public:
  FilterDesignView();
  ~FilterDesignView();

  FilterDesignView(const FilterDesignView&) = delete;
  FilterDesignView& operator=(const FilterDesignView&) = delete;

  /**
   * Draws the panel.
   *
   * @param inferredSampleRate If >0, shown as a "use inferred fs" shortcut.
   */
  void Display(double inferredSampleRate = 0.0);

  /**
   * @return The combined cascade across all stages, or nullopt if there are
   *         no stages or any stage's parameters are invalid.
   */
  const std::optional<Sections>& Result() const { return m_result; }

  /** @return The sample rate currently configured for the chain. */
  double SampleRate() const { return m_sampleRate; }

  /**
   * @return A counter that increments every time the chain is re-designed,
   *         suitable as a cache key for downstream consumers.
   */
  uint64_t Version() const { return m_version; }

  /**
   * Builds a multi-line, human-readable description of the current chain
   * (sample rate + per-stage parameters). No comment markers — callers
   * apply language-specific prefixes when embedding in generated source.
   */
  std::string Describe() const;

 private:
  void Redesign();
  void PollSaveDialog();
  void PollOpenDialog();
  void DoSave(const std::string& path);
  void DoOpen(const std::string& path);

  std::vector<Stage> m_stages;
  std::vector<bool> m_stageValid;
  double m_sampleRate = 1000.0;

  std::optional<Sections> m_result;
  bool m_dirty = true;
  uint64_t m_version = 0;

  std::unique_ptr<pfd::save_file> m_savePicker;
  std::unique_ptr<pfd::open_file> m_openPicker;
  std::string m_persistError;
  std::string m_persistInfo;
};

}  // namespace wpi::filterdesigner
