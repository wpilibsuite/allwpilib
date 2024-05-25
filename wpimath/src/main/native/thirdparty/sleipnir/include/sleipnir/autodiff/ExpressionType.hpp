// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

namespace sleipnir {

/**
 * Expression type.
 *
 * Used for autodiff caching.
 */
enum class ExpressionType : uint8_t {
  /// There is no expression.
  kNone,
  /// The expression is a constant.
  kConstant,
  /// The expression is composed of linear and lower-order operators.
  kLinear,
  /// The expression is composed of quadratic and lower-order operators.
  kQuadratic,
  /// The expression is composed of nonlinear and lower-order operators.
  kNonlinear
};

}  // namespace sleipnir
