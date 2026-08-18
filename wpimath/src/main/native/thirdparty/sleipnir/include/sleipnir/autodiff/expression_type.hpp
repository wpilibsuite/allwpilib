// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

#include <format>
#include <utility>

namespace slp {

/// Expression type.
///
/// Used for autodiff caching.
enum class ExpressionType : uint8_t {
  /// There is no expression.
  NONE,
  /// The expression is a constant.
  CONSTANT,
  /// The expression is composed of linear and lower-order operators.
  LINEAR,
  /// The expression is composed of quadratic and lower-order operators.
  QUADRATIC,
  /// The expression is composed of nonlinear and lower-order operators.
  NONLINEAR
};

}  // namespace slp

/// Formatter for ExpressionType.
template <>
struct std::formatter<slp::ExpressionType> {
  /// Parse format string.
  ///
  /// @param ctx Format parse context.
  /// @return Format parse context iterator.
  constexpr auto parse(std::format_parse_context& ctx) {
    return m_underlying.parse(ctx);
  }

  /// Formats ExpressionType.
  ///
  /// @tparam FmtContext Format context type.
  /// @param type Expression type.
  /// @param ctx Format context.
  /// @return Format context iterator.
  template <typename FmtContext>
  auto format(const slp::ExpressionType& type, FmtContext& ctx) const {
    using enum slp::ExpressionType;

    switch (type) {
      case NONE:
        return m_underlying.format("none", ctx);
      case CONSTANT:
        return m_underlying.format("constant", ctx);
      case LINEAR:
        return m_underlying.format("linear", ctx);
      case QUADRATIC:
        return m_underlying.format("quadratic", ctx);
      case NONLINEAR:
        return m_underlying.format("nonlinear", ctx);
      default:
        std::unreachable();
    }
  }

 private:
  std::formatter<const char*> m_underlying;
};
