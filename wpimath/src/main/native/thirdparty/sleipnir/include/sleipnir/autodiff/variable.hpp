// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <source_location>
#include <type_traits>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/expression.hpp"
#include "sleipnir/autodiff/expression_graph.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/concepts.hpp"
#include "sleipnir/util/symbol_exports.hpp"

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
#include "sleipnir/util/print.hpp"
#endif

namespace slp {

// Forward declarations for friend declarations in Variable
namespace detail {
class AdjointExpressionGraph;
}  // namespace detail
template <int UpLo = Eigen::Lower | Eigen::Upper>
  requires(UpLo == Eigen::Lower) || (UpLo == (Eigen::Lower | Eigen::Upper))
class SLEIPNIR_DLLEXPORT Hessian;
class SLEIPNIR_DLLEXPORT Jacobian;

/**
 * An autodiff variable pointing to an expression node.
 */
class SLEIPNIR_DLLEXPORT Variable {
 public:
  /**
   * Constructs a linear Variable with a value of zero.
   */
  Variable() = default;

  /**
   * Constructs an empty Variable.
   */
  explicit Variable(std::nullptr_t) : expr{nullptr} {}

  /**
   * Constructs a Variable from a floating point type.
   *
   * @param value The value of the Variable.
   */
  Variable(std::floating_point auto value)  // NOLINT
      : expr{detail::make_expression_ptr<detail::ConstExpression>(value)} {}

  /**
   * Constructs a Variable from an integral type.
   *
   * @param value The value of the Variable.
   */
  Variable(std::integral auto value)  // NOLINT
      : expr{detail::make_expression_ptr<detail::ConstExpression>(value)} {}

  /**
   * Constructs a Variable pointing to the specified expression.
   *
   * @param expr The autodiff variable.
   */
  explicit Variable(const detail::ExpressionPtr& expr) : expr{expr} {}

  /**
   * Constructs a Variable pointing to the specified expression.
   *
   * @param expr The autodiff variable.
   */
  explicit Variable(detail::ExpressionPtr&& expr) : expr{std::move(expr)} {}

  /**
   * Assignment operator for double.
   *
   * @param value The value of the Variable.
   * @return This variable.
   */
  Variable& operator=(double value) {
    expr = detail::make_expression_ptr<detail::ConstExpression>(value);

    return *this;
  }

  /**
   * Sets Variable's internal value.
   *
   * @param value The value of the Variable.
   */
  void set_value(double value) {
    if (expr->is_constant(0.0)) {
      expr = detail::make_expression_ptr<detail::ConstExpression>(value);
    } else {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
      // We only need to check the first argument since unary and binary
      // operators both use it
      if (expr->args[0] != nullptr) {
        auto location = std::source_location::current();
        slp::println(
            stderr,
            "WARNING: {}:{}: {}: Modified the value of a dependent variable",
            location.file_name(), location.line(), location.function_name());
      }
#endif
      expr->val = value;
    }
  }

  /**
   * Variable-Variable multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of multiplication.
   */
  friend SLEIPNIR_DLLEXPORT Variable operator*(const Variable& lhs,
                                               const Variable& rhs) {
    return Variable{lhs.expr * rhs.expr};
  }

  /**
   * Variable-Variable compound multiplication operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of multiplication.
   */
  Variable& operator*=(const Variable& rhs) {
    *this = *this * rhs;
    return *this;
  }

  /**
   * Variable-Variable division operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  friend SLEIPNIR_DLLEXPORT Variable operator/(const Variable& lhs,
                                               const Variable& rhs) {
    return Variable{lhs.expr / rhs.expr};
  }

  /**
   * Variable-Variable compound division operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of division.
   */
  Variable& operator/=(const Variable& rhs) {
    *this = *this / rhs;
    return *this;
  }

  /**
   * Variable-Variable addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  friend SLEIPNIR_DLLEXPORT Variable operator+(const Variable& lhs,
                                               const Variable& rhs) {
    return Variable{lhs.expr + rhs.expr};
  }

  /**
   * Variable-Variable compound addition operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of addition.
   */
  Variable& operator+=(const Variable& rhs) {
    *this = *this + rhs;
    return *this;
  }

  /**
   * Variable-Variable subtraction operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  friend SLEIPNIR_DLLEXPORT Variable operator-(const Variable& lhs,
                                               const Variable& rhs) {
    return Variable{lhs.expr - rhs.expr};
  }

  /**
   * Variable-Variable compound subtraction operator.
   *
   * @param rhs Operator right-hand side.
   * @return Result of subtraction.
   */
  Variable& operator-=(const Variable& rhs) {
    *this = *this - rhs;
    return *this;
  }

  /**
   * Unary minus operator.
   *
   * @param lhs Operand for unary minus.
   */
  friend SLEIPNIR_DLLEXPORT Variable operator-(const Variable& lhs) {
    return Variable{-lhs.expr};
  }

  /**
   * Unary plus operator.
   *
   * @param lhs Operand for unary plus.
   */
  friend SLEIPNIR_DLLEXPORT Variable operator+(const Variable& lhs) {
    return Variable{+lhs.expr};
  }

  /**
   * Returns the value of this variable.
   *
   * @return The value of this variable.
   */
  double value() {
    if (!m_graph_initialized) {
      m_graph = detail::topological_sort(expr);
      m_graph_initialized = true;
    }
    detail::update_values(m_graph);

    return expr->val;
  }

  /**
   * Returns the type of this expression (constant, linear, quadratic, or
   * nonlinear).
   *
   * @return The type of this expression.
   */
  ExpressionType type() const { return expr->type(); }

 private:
  /// The expression node
  detail::ExpressionPtr expr =
      detail::make_expression_ptr<detail::DecisionVariableExpression>();

  /// Used to update the value of this variable based on the values of its
  /// dependent variables
  gch::small_vector<detail::Expression*> m_graph;

  /// Used for lazy initialization of m_graph
  bool m_graph_initialized = false;

  friend SLEIPNIR_DLLEXPORT Variable abs(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable acos(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable asin(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable atan(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable atan2(const Variable& y,
                                           const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable cos(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable cosh(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable erf(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable exp(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable hypot(const Variable& x,
                                           const Variable& y);
  friend SLEIPNIR_DLLEXPORT Variable log(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable log10(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable pow(const Variable& base,
                                         const Variable& power);
  friend SLEIPNIR_DLLEXPORT Variable sign(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable sin(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable sinh(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable sqrt(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable tan(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable tanh(const Variable& x);
  friend SLEIPNIR_DLLEXPORT Variable hypot(const Variable& x, const Variable& y,
                                           const Variable& z);

  friend class detail::AdjointExpressionGraph;
  template <int UpLo>
    requires(UpLo == Eigen::Lower) || (UpLo == (Eigen::Lower | Eigen::Upper))
  friend class SLEIPNIR_DLLEXPORT Hessian;
  friend class SLEIPNIR_DLLEXPORT Jacobian;
};

/**
 * std::abs() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable abs(const Variable& x) {
  return Variable{detail::abs(x.expr)};
}

/**
 * std::acos() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable acos(const Variable& x) {
  return Variable{detail::acos(x.expr)};
}

/**
 * std::asin() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable asin(const Variable& x) {
  return Variable{detail::asin(x.expr)};
}

/**
 * std::atan() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable atan(const Variable& x) {
  return Variable{detail::atan(x.expr)};
}

/**
 * std::atan2() for Variables.
 *
 * @param y The y argument.
 * @param x The x argument.
 */
SLEIPNIR_DLLEXPORT inline Variable atan2(const Variable& y, const Variable& x) {
  return Variable{detail::atan2(y.expr, x.expr)};
}

/**
 * std::cos() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable cos(const Variable& x) {
  return Variable{detail::cos(x.expr)};
}

/**
 * std::cosh() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable cosh(const Variable& x) {
  return Variable{detail::cosh(x.expr)};
}

/**
 * std::erf() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable erf(const Variable& x) {
  return Variable{detail::erf(x.expr)};
}

/**
 * std::exp() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable exp(const Variable& x) {
  return Variable{detail::exp(x.expr)};
}

/**
 * std::hypot() for Variables.
 *
 * @param x The x argument.
 * @param y The y argument.
 */
SLEIPNIR_DLLEXPORT inline Variable hypot(const Variable& x, const Variable& y) {
  return Variable{detail::hypot(x.expr, y.expr)};
}

/**
 * std::pow() for Variables.
 *
 * @param base The base.
 * @param power The power.
 */
SLEIPNIR_DLLEXPORT inline Variable pow(const Variable& base,
                                       const Variable& power) {
  return Variable{detail::pow(base.expr, power.expr)};
}

/**
 * std::log() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable log(const Variable& x) {
  return Variable{detail::log(x.expr)};
}

/**
 * std::log10() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable log10(const Variable& x) {
  return Variable{detail::log10(x.expr)};
}

/**
 * sign() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable sign(const Variable& x) {
  return Variable{detail::sign(x.expr)};
}

/**
 * std::sin() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable sin(const Variable& x) {
  return Variable{detail::sin(x.expr)};
}

/**
 * std::sinh() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable sinh(const Variable& x) {
  return Variable{detail::sinh(x.expr)};
}

/**
 * std::sqrt() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable sqrt(const Variable& x) {
  return Variable{detail::sqrt(x.expr)};
}

/**
 * std::tan() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable tan(const Variable& x) {
  return Variable{detail::tan(x.expr)};
}

/**
 * std::tanh() for Variables.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline Variable tanh(const Variable& x) {
  return Variable{detail::tanh(x.expr)};
}

/**
 * std::hypot() for Variables.
 *
 * @param x The x argument.
 * @param y The y argument.
 * @param z The z argument.
 */
SLEIPNIR_DLLEXPORT inline Variable hypot(const Variable& x, const Variable& y,
                                         const Variable& z) {
  return Variable{slp::sqrt(slp::pow(x, 2) + slp::pow(y, 2) + slp::pow(z, 2))};
}

/**
 * Make a list of constraints.
 *
 * The standard form for equality constraints is c(x) = 0, and the standard form
 * for inequality constraints is c(x) ≥ 0. This function takes constraints of
 * the form lhs = rhs or lhs ≥ rhs and converts them to lhs - rhs = 0 or
 * lhs - rhs ≥ 0.
 *
 * @param lhs Left-hand side.
 * @param rhs Right-hand side.
 */
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
gch::small_vector<Variable> make_constraints(LHS&& lhs, RHS&& rhs) {
  gch::small_vector<Variable> constraints;

  if constexpr (ScalarLike<LHS> && ScalarLike<RHS>) {
    constraints.emplace_back(lhs - rhs);
  } else if constexpr (ScalarLike<LHS> && MatrixLike<RHS>) {
    constraints.reserve(rhs.rows() * rhs.cols());

    for (int row = 0; row < rhs.rows(); ++row) {
      for (int col = 0; col < rhs.cols(); ++col) {
        // Make right-hand side zero
        constraints.emplace_back(lhs - rhs(row, col));
      }
    }
  } else if constexpr (MatrixLike<LHS> && ScalarLike<RHS>) {
    constraints.reserve(lhs.rows() * lhs.cols());

    for (int row = 0; row < lhs.rows(); ++row) {
      for (int col = 0; col < lhs.cols(); ++col) {
        // Make right-hand side zero
        constraints.emplace_back(lhs(row, col) - rhs);
      }
    }
  } else if constexpr (MatrixLike<LHS> && MatrixLike<RHS>) {
    slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());
    constraints.reserve(lhs.rows() * lhs.cols());

    for (int row = 0; row < lhs.rows(); ++row) {
      for (int col = 0; col < lhs.cols(); ++col) {
        // Make right-hand side zero
        constraints.emplace_back(lhs(row, col) - rhs(row, col));
      }
    }
  }

  return constraints;
}

/**
 * A vector of equality constraints of the form cₑ(x) = 0.
 */
struct SLEIPNIR_DLLEXPORT EqualityConstraints {
  /// A vector of scalar equality constraints.
  gch::small_vector<Variable> constraints;

  /**
   * Concatenates multiple equality constraints.
   *
   * @param equality_constraints The list of EqualityConstraints to concatenate.
   */
  EqualityConstraints(
      std::initializer_list<EqualityConstraints> equality_constraints) {
    for (const auto& elem : equality_constraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /**
   * Concatenates multiple equality constraints.
   *
   * This overload is for Python bindings only.
   *
   * @param equality_constraints The list of EqualityConstraints to concatenate.
   */
  explicit EqualityConstraints(
      const std::vector<EqualityConstraints>& equality_constraints) {
    for (const auto& elem : equality_constraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /**
   * Constructs an equality constraint from a left and right side.
   *
   * The standard form for equality constraints is c(x) = 0. This function takes
   * a constraint of the form lhs = rhs and converts it to lhs - rhs = 0.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   */
  template <typename LHS, typename RHS>
    requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
            (ScalarLike<RHS> || MatrixLike<RHS>) &&
            (SleipnirType<LHS> || SleipnirType<RHS>)
  EqualityConstraints(LHS&& lhs, RHS&& rhs)
      : constraints{make_constraints(lhs, rhs)} {}

  /**
   * Implicit conversion operator to bool.
   */
  operator bool() {  // NOLINT
    return std::ranges::all_of(constraints, [](auto& constraint) {
      return constraint.value() == 0.0;
    });
  }
};

/**
 * A vector of inequality constraints of the form cᵢ(x) ≥ 0.
 */
struct SLEIPNIR_DLLEXPORT InequalityConstraints {
  /// A vector of scalar inequality constraints.
  gch::small_vector<Variable> constraints;

  /**
   * Concatenates multiple inequality constraints.
   *
   * @param inequality_constraints The list of InequalityConstraints to
   * concatenate.
   */
  InequalityConstraints(  // NOLINT
      std::initializer_list<InequalityConstraints> inequality_constraints) {
    for (const auto& elem : inequality_constraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /**
   * Concatenates multiple inequality constraints.
   *
   * This overload is for Python bindings only.
   *
   * @param inequality_constraints The list of InequalityConstraints to
   * concatenate.
   */
  explicit InequalityConstraints(
      const std::vector<InequalityConstraints>& inequality_constraints) {
    for (const auto& elem : inequality_constraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /**
   * Constructs an inequality constraint from a left and right side.
   *
   * The standard form for inequality constraints is c(x) ≥ 0. This function
   * takes a constraints of the form lhs ≥ rhs and converts it to lhs - rhs ≥ 0.
   *
   * @param lhs Left-hand side.
   * @param rhs Right-hand side.
   */
  template <typename LHS, typename RHS>
    requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
            (ScalarLike<RHS> || MatrixLike<RHS>) &&
            (SleipnirType<LHS> || SleipnirType<RHS>)
  InequalityConstraints(LHS&& lhs, RHS&& rhs)
      : constraints{make_constraints(lhs, rhs)} {}

  /**
   * Implicit conversion operator to bool.
   */
  operator bool() {  // NOLINT
    return std::ranges::all_of(constraints, [](auto& constraint) {
      return constraint.value() >= 0.0;
    });
  }
};

/**
 * Equality operator that returns an equality constraint for two Variables.
 *
 * @param lhs Left-hand side.
 * @param rhs Left-hand side.
 */
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
EqualityConstraints operator==(LHS&& lhs, RHS&& rhs) {
  return EqualityConstraints{lhs, rhs};
}

/**
 * Less-than comparison operator that returns an inequality constraint for two
 * Variables.
 *
 * @param lhs Left-hand side.
 * @param rhs Left-hand side.
 */
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
InequalityConstraints operator<(LHS&& lhs, RHS&& rhs) {
  return rhs >= lhs;
}

/**
 * Less-than-or-equal-to comparison operator that returns an inequality
 * constraint for two Variables.
 *
 * @param lhs Left-hand side.
 * @param rhs Left-hand side.
 */
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
InequalityConstraints operator<=(LHS&& lhs, RHS&& rhs) {
  return rhs >= lhs;
}

/**
 * Greater-than comparison operator that returns an inequality constraint for
 * two Variables.
 *
 * @param lhs Left-hand side.
 * @param rhs Left-hand side.
 */
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
InequalityConstraints operator>(LHS&& lhs, RHS&& rhs) {
  return lhs >= rhs;
}

/**
 * Greater-than-or-equal-to comparison operator that returns an inequality
 * constraint for two Variables.
 *
 * @param lhs Left-hand side.
 * @param rhs Left-hand side.
 */
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
InequalityConstraints operator>=(LHS&& lhs, RHS&& rhs) {
  return InequalityConstraints{lhs, rhs};
}

}  // namespace slp

namespace Eigen {

/**
 * NumTraits specialization that allows instantiating Eigen types with Variable.
 */
template <>
struct NumTraits<slp::Variable> : NumTraits<double> {
  /// Real type.
  using Real = slp::Variable;
  /// Non-integer type.
  using NonInteger = slp::Variable;
  /// Nested type.
  using Nested = slp::Variable;

  /// Is complex.
  static constexpr int IsComplex = 0;
  /// Is integer.
  static constexpr int IsInteger = 0;
  /// Is signed.
  static constexpr int IsSigned = 1;
  /// Require initialization.
  static constexpr int RequireInitialization = 1;
  /// Read cost.
  static constexpr int ReadCost = 1;
  /// Add cost.
  static constexpr int AddCost = 3;
  /// Multiply cost.
  static constexpr int MulCost = 3;
};

}  // namespace Eigen
