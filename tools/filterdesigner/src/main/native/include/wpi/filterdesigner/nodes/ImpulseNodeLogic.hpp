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
 * The UI mutates @ref sampleRate and @ref length in place; @ref Signal rebuilds
 * lazily when either changes, bumping the signal's revision so downstream
 * caches see the new content.
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
   * Returns a pointer to the cached Signal, rebuilding it when @ref sampleRate
   * or @ref length have changed and invalidating the previous pointer. Returns
   * nullptr for a non-positive rate or a length below @ref kMinLength.
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
