// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) state of a BiquadStageNode: the per-stage design parameters
 * plus the cached design output and the cached filtered Signal. Exposes
 * stable pointers suitable for ImNodeFlow's pull model.
 *
 * The UI mutates `stage` and `sampleRate` in place; both caches are lazy and
 * re-evaluate when they change. The filtered signal is additionally keyed on
 * (input pointer, input revision, filter version), so a steady state costs
 * three integer compares.
 */
class BiquadStageNodeLogic {
 public:
  BiquadStageNodeLogic() = default;

  BiquadStageNodeLogic(const BiquadStageNodeLogic&) = delete;
  BiquadStageNodeLogic& operator=(const BiquadStageNodeLogic&) = delete;

  /** Per-stage design parameters, mutated in place by the UI. */
  Stage stage;

  /** Sample rate (Hz) the filter is designed at, and emitted on the wire. */
  double sampleRate = 1000.0;

  /**
   * When true, @ref sampleRate mirrors the connected input's rate. Hand-
   * editing the field clears it, the node's @c Auto checkbox restores it.
   * Default true: the 1000 Hz default silently shifts every cutoff by 5x on a
   * 200 Hz stream.
   */
  bool sampleRateAutoSync = true;

  /**
   * Relative deadband the node applies before adopting an input's rate in
   * auto mode: a live source's inferred rate wobbles by well under a percent
   * as samples roll through its ring buffer, and the design cache keys on
   * exact equality, so adopting every wobble would redesign the filter and
   * invalidate every downstream cache each frame.
   */
  static constexpr double kRateSyncTolerance = 0.01;

  /**
   * Relative mismatch between an input's rate and @ref sampleRate beyond
   * which @ref Filtered refuses to run. Twice the sync deadband, so a rate
   * the node is about to adopt never trips it for a frame.
   */
  static constexpr double kRateMismatchTolerance = 2.0 * kRateSyncTolerance;

  /**
   * Designs the filter from the current @ref stage and @ref sampleRate and
   * returns a pointer to the cached result, valid until the next redesign.
   * Returns nullptr for parameters the family rejects (a cutoff above Nyquist,
   * say), with the message in @ref DesignError.
   */
  const DesignedFilter* Filter() const;

  /**
   * Returns @p input filtered through the current cascade, or nullptr if
   * @p input is null, the design is invalid, or the input's rate differs from
   * @ref sampleRate by more than @ref kRateMismatchTolerance — the
   * coefficients only mean what the Bode plot says at the rate they were
   * designed for. The result is cached while (input pointer, input revision,
   * filter version) hold, and the pointer is valid until they don't.
   *
   * Timestamps carry over verbatim; the name gains a stage suffix so a
   * multi-stage chain reads clearly in a plot legend.
   */
  const Signal* Filtered(const Signal* input) const;

  /**
   * Human-readable design error from the most recent @ref Filter call.
   * Empty when the current params produced a valid cascade.
   */
  const std::string& DesignError() const { return m_designError; }

  /**
   * Why the most recent @ref Filtered call returned nullptr for a non-null
   * input with a valid design; empty otherwise. Today that is only a rate
   * mismatch.
   */
  const std::string& FilterError() const { return m_filterError; }

  /**
   * Monotonically increasing version that bumps every time the design
   * changes. Downstream caches can compare this to detect filter churn.
   */
  std::uint64_t FilterVersion() const { return m_filterVersion; }

 private:
  /** Returns true if (stage, sampleRate) match the last-designed values. */
  bool DesignCacheIsFresh() const;

  // Cache for Filter(). m_designedStage/Fs are the (stage, sampleRate) that
  // produced m_designCache; comparing against the current values is how we
  // detect param changes without an explicit "dirty" signal from the UI.
  mutable std::optional<DesignedFilter> m_designCache;
  mutable Stage m_designedStage{};
  mutable double m_designedFs = 0.0;
  mutable bool m_haveDesigned = false;
  mutable std::string m_designError;
  mutable std::uint64_t m_filterVersion = 0;

  // Cache for Filtered(). The key (input ptr, input revision, filter
  // version) is enough — Signal::revision bumps in place when sources
  // mutate their buffer (NT4 ring-buffer), and a redesign bumps
  // m_filterVersion.
  mutable std::optional<Signal> m_filteredCache;
  mutable const Signal* m_filteredInput = nullptr;
  mutable std::uint64_t m_filteredInputRev = 0;
  mutable std::uint64_t m_filteredAtFilterVersion = 0;
  mutable bool m_haveFiltered = false;
  mutable std::string m_filterError;
  // Revision stamped onto the filtered output; m_filteredCache is
  // pointer-stable, so downstream caches key on this to see recomputes.
  mutable std::uint64_t m_outRevision = 0;
};

}  // namespace wpi::filterdesigner
