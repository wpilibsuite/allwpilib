// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

namespace slp {

/// Enum describing the type of system timestep.
enum class TimestepMethod : uint8_t {
  /// The timestep is a fixed constant.
  FIXED,
  /// The timesteps are allowed to vary as independent decision variables.
  VARIABLE,
  /// The timesteps are equal length but allowed to vary as a single decision
  /// variable.
  VARIABLE_SINGLE
};

}  // namespace slp
