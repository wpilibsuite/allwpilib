// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

namespace slp {

/**
 * Enum describing an OCP transcription method.
 */
enum class TranscriptionMethod : uint8_t {
  /// Each state is a decision variable constrained to the integrated dynamics
  /// of the previous state.
  DIRECT_TRANSCRIPTION,
  /// The trajectory is modeled as a series of cubic polynomials where the
  /// centerpoint slope is constrained.
  DIRECT_COLLOCATION,
  /// States depend explicitly as a function of all previous states and all
  /// previous inputs.
  SINGLE_SHOOTING
};

}  // namespace slp
