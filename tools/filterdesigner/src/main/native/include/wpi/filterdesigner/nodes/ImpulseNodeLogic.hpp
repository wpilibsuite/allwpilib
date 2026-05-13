// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <optional>

#include "wpi/filterdesigner/model/Signal.hpp"

namespace wpi::filterdesigner {

/**
 * Pure (UI-free) state of an ImpulseNode: a discrete unit impulse
 * @c [1, 0, 0, ...] of @ref length samples at @ref sampleRate Hz. Exposes
 * a stable Signal pointer for ImNodeFlow's pull model.
 *
 * UI mutates @ref sampleRate / @ref length in place; @ref Signal is lazy and
 * rebuilds when either changes. The cached @c Signal bumps its @c revision
 * on every rebuild so downstream caches (BiquadStage's filtered-signal
 * cache) see the new content.
 */
class ImpulseNodeLogic {
 public:
  ImpulseNodeLogic() = default;

  ImpulseNodeLogic(const ImpulseNodeLogic&) = delete;
  ImpulseNodeLogic& operator=(const ImpulseNodeLogic&) = delete;

  /** Sampling rate in Hz. Carries through onto the produced Signal. */
  double sampleRate = 1000.0;

  /** Number of samples in the produced impulse. Clamped on read. */
  int length = 200;

  /**
   * Returns a stable pointer to the cached Signal, rebuilding if @ref
   * sampleRate / @ref length have changed since the last call. Returns
   * nullptr if the parameters are invalid (non-positive sample rate or
   * length below @ref kMinLength).
   *
   * Lifetime: pointer is valid until the next call that triggers a rebuild.
   */
  const wpi::filterdesigner::Signal* Signal() const;

  static constexpr int kMinLength = 2;
  static constexpr int kMaxLength = 1 << 16;

 private:
  mutable std::optional<wpi::filterdesigner::Signal> m_signal;
  mutable double m_lastSampleRate = 0.0;
  mutable int m_lastLength = 0;
  mutable std::uint64_t m_revision = 0;
};

}  // namespace wpi::filterdesigner
