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
 * Three pieces of state:
 *   - `stage` + `sampleRate` — the design parameters. UI mutates them in
 *     place; @ref Filter is lazy and re-evaluates when any of them changes.
 *   - cached @ref DesignedFilter — the cascade + the fs it was designed at,
 *     handed downstream through the node's Filter output pin.
 *   - cached filtered @ref Signal — input run through the cascade, served on
 *     the node's Signal output pin. Keyed on (input pointer, input revision,
 *     filter version) so steady-state cost is three integer compares.
 */
class BiquadStageNodeLogic {
 public:
  BiquadStageNodeLogic() = default;

  BiquadStageNodeLogic(const BiquadStageNodeLogic&) = delete;
  BiquadStageNodeLogic& operator=(const BiquadStageNodeLogic&) = delete;

  /**
   * Per-stage design parameters. The UI mutates these in place. @ref Filter
   * compares them against the last-designed values to decide whether to
   * redesign.
   */
  Stage stage;

  /**
   * Sample rate (Hz) the filter is designed at. Used by both the design
   * factories and emitted on the Filter wire so downstream sinks see the
   * same fs the cascade was built for.
   */
  double sampleRate = 1000.0;

  /**
   * When true, @ref sampleRate mirrors the connected input Signal's
   * @c sampleRate every frame. Hand-editing the field clears this; the
   * @c Auto checkbox in the node re-enables it. Default true so a stage
   * dropped onto a connected source picks up the source's rate without an
   * extra click — without it, a 1000 Hz default applied to a 200 Hz NT4
   * stream silently shifts every cutoff by 5x.
   */
  bool sampleRateAutoSync = true;

  /**
   * Designs the filter from the current @ref stage + @ref sampleRate, caches
   * the result, and returns a stable pointer to it. Returns nullptr when the
   * current parameters are out-of-range for the selected family (e.g.
   * cutoff above Nyquist); @ref DesignError carries the user-facing message
   * in that case.
   *
   * Lifetime: pointer is valid until the next call to @ref Filter that
   * triggers a redesign (i.e. when stage or sampleRate changes).
   */
  const DesignedFilter* Filter() const;

  /**
   * Returns @p input filtered through the current cascade. Returns nullptr
   * if @p input is null or if the filter design is invalid. The result is
   * cached and reused while (input pointer, input revision, filter version)
   * are unchanged.
   *
   * Output keeps the input's timestamps array verbatim; only the values
   * change. The output's @c name is the input name plus a suffix so multi-
   * stage chains read clearly in plot legends.
   *
   * Lifetime: pointer is valid until the next call that invalidates the
   * cache (different input pointer, different revision, or filter
   * redesign).
   */
  const Signal* Filtered(const Signal* input) const;

  /**
   * Human-readable design error from the most recent @ref Filter call.
   * Empty when the current params produced a valid cascade.
   */
  const std::string& DesignError() const { return m_designError; }

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
};

}  // namespace wpi::filterdesigner
