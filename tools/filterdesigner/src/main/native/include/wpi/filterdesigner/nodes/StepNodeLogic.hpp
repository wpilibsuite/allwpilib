// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <optional>

#include "wpi/filterdesigner/model/Signal.hpp"

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) state of a StepNode: a unit step Signal — @c [0, ..., 0,
 * 1, 1, ...] of @ref length samples at @ref sampleRate Hz, transitioning
 * from zero to one at sample index @ref startSample. Mirrors the synth half
 * of the linear-chain @c TimeResponseView, exposed through a stable pointer
 * for ImNodeFlow's pull model.
 */
class StepNodeLogic {
 public:
  StepNodeLogic() = default;

  StepNodeLogic(const StepNodeLogic&) = delete;
  StepNodeLogic& operator=(const StepNodeLogic&) = delete;

  /** Sampling rate in Hz. Carries through onto the produced Signal. */
  double sampleRate = 1000.0;

  /** Number of samples in the produced step. Clamped on read. */
  int length = 200;

  /**
   * Sample index at which the step transitions from 0 to 1. 0 = unit step
   * starting at the first sample (matches the linear-chain @c
   * TimeResponseView). Clamped into [0, length).
   */
  int startSample = 0;

  /**
   * Returns a stable pointer to the cached Signal, rebuilding if @ref
   * sampleRate / @ref length / @ref startSample have changed since the last
   * call. Returns nullptr if the parameters are invalid (non-positive sample
   * rate or length below @ref kMinLength).
   */
  const wpi::filterdesigner::Signal* Signal() const;

  /** Bumps every time the cached Signal is rebuilt. */
  std::uint64_t Revision() const { return m_revision; }

  static constexpr int kMinLength = 2;
  static constexpr int kMaxLength = 1 << 16;

 private:
  mutable std::optional<wpi::filterdesigner::Signal> m_signal;
  mutable double m_lastSampleRate = 0.0;
  mutable int m_lastLength = 0;
  mutable int m_lastStartSample = -1;
  mutable std::uint64_t m_revision = 0;
};

}  // namespace wpi::filterdesigner
