// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

namespace slp {

/// Enum describing a type of system dynamics constraints.
enum class DynamicsType : uint8_t {
  /// The dynamics are a function in the form dx/dt = f(t, x, u).
  EXPLICIT_ODE,
  /// The dynamics are a function in the form xₖ₊₁ = f(t, xₖ, uₖ).
  DISCRETE
};

}  // namespace slp
