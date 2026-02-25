// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <source_location>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/expression.hpp"
#include "sleipnir/autodiff/expression_graph.hpp"
#include "sleipnir/autodiff/sleipnir_base.hpp"
#include "sleipnir/util/assert.hpp"
#include "sleipnir/util/concepts.hpp"

#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
#include "sleipnir/util/print.hpp"
#endif

namespace slp {

// Forward declarations for friend declarations in Variable

namespace detail {

template <typename Scalar>
class GradientExpressionGraph;

}  // namespace detail

template <typename Scalar, int UpLo = Eigen::Lower | Eigen::Upper>
  requires(UpLo == Eigen::Lower) || (UpLo == (Eigen::Lower | Eigen::Upper))
class Hessian;

template <typename Scalar>
class Jacobian;

/// An autodiff variable pointing to an expression node.
///
/// @tparam Scalar_ Scalar type.
template <typename Scalar_>
class Variable : public SleipnirBase {
 public:
  /// Scalar type alias.
  using Scalar = Scalar_;

  /// Constructs a linear Variable with a value of zero.
  Variable() = default;

  /// Constructs an empty Variable.
  explicit Variable(std::nullptr_t) : expr{nullptr} {}

  /// Constructs a Variable from a scalar type.
  ///
  /// @param value The value of the Variable.
  // NOLINTNEXTLINE (google-explicit-constructor)
  Variable(Scalar value)
    requires(!MatrixLike<Scalar>)
      : expr{detail::make_expression_ptr<detail::ConstantExpression<Scalar>>(
            value)} {}

  /// Constructs a Variable from a scalar type.
  ///
  /// @param value The value of the Variable.
  // NOLINTNEXTLINE (google-explicit-constructor)
  Variable(SleipnirMatrixLike<Scalar> auto value) : expr{value(0, 0).expr} {
    slp_assert(value.rows() == 1 && value.cols() == 1);
  }

  /// Constructs a Variable from a floating-point type.
  ///
  /// @param value The value of the Variable.
  // NOLINTNEXTLINE (google-explicit-constructor)
  Variable(std::floating_point auto value)
      : expr{detail::make_expression_ptr<detail::ConstantExpression<Scalar>>(
            Scalar(value))} {}

  /// Constructs a Variable from an integral type.
  ///
  /// @param value The value of the Variable.
  // NOLINTNEXTLINE (google-explicit-constructor)
  Variable(std::integral auto value)
      : expr{detail::make_expression_ptr<detail::ConstantExpression<Scalar>>(
            Scalar(value))} {}

  /// Constructs a Variable pointing to the specified expression.
  ///
  /// @param expr The autodiff variable.
  explicit Variable(const detail::ExpressionPtr<Scalar>& expr) : expr{expr} {}

  /// Constructs a Variable pointing to the specified expression.
  ///
  /// @param expr The autodiff variable.
  explicit Variable(detail::ExpressionPtr<Scalar>&& expr)
      : expr{std::move(expr)} {}

  /// Assignment operator for scalar.
  ///
  /// @param value The value of the Variable.
  /// @return This variable.
  Variable<Scalar>& operator=(ScalarLike auto value) {
    expr =
        detail::make_expression_ptr<detail::ConstantExpression<Scalar>>(value);
    m_graph_initialized = false;

    return *this;
  }

  /// Sets Variable's internal value.
  ///
  /// @param value The value of the Variable.
  void set_value(Scalar value) {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    // We only need to check the first argument since unary and binary operators
    // both use it
    if (expr->args[0] != nullptr) {
      auto location = std::source_location::current();
      slp::println(
          stderr,
          "WARNING: {}:{}: {}: Modified the value of a dependent variable",
          location.file_name(), location.line(), location.function_name());
    }
#endif
    expr->val = Scalar(value);
  }

  /// Returns the value of this variable.
  ///
  /// @return The value of this variable.
  Scalar value() {
    if (!m_graph_initialized) {
      m_graph = detail::topological_sort(expr);
      m_graph_initialized = true;
    }
    detail::update_values(m_graph);

    return Scalar(expr->val);
  }

  /// Returns the type of this expression (constant, linear, quadratic, or
  /// nonlinear).
  ///
  /// @return The type of this expression.
  ExpressionType type() const { return expr->type(); }

  /// Variable-scalar multiplication operator.
  ///
  /// @param lhs Operator left-hand side.
  /// @param rhs Operator right-hand side.
  /// @return Result of multiplication.
  template <ScalarLike LHS, SleipnirScalarLike<Scalar> RHS>
  friend Variable<Scalar> operator*(const LHS& lhs, const RHS& rhs) {
    return Variable{Variable<Scalar>{lhs}.expr * rhs.expr};
  }

  /// Variable-scalar multiplication operator.
  ///
  /// @param lhs Operator left-hand side.
  /// @param rhs Operator right-hand side.
  /// @return Result of multiplication.
  template <SleipnirScalarLike<Scalar> LHS, ScalarLike RHS>
  friend Variable<Scalar> operator*(const LHS& lhs, const RHS& rhs) {
    return Variable{lhs.expr * Variable<Scalar>{rhs}.expr};
  }

  /// Variable-scalar multiplication operator.
  ///
  /// @param lhs Operator left-hand side.
  /// @param rhs Operator right-hand side.
  /// @return Result of multiplication.
  friend Variable<Scalar> operator*(const Variable<Scalar>& lhs,
                                    const Variable<Scalar>& rhs) {
    return Variable{lhs.expr * rhs.expr};
  }

  /// Variable-Variable compound multiplication operator.
  ///
  /// @param rhs Operator right-hand side.
  /// @return Result of multiplication.
  Variable<Scalar>& operator*=(const Variable<Scalar>& rhs) {
    *this = *this * rhs;
    return *this;
  }

  /// Variable-Variable division operator.
  ///
  /// @param lhs Operator left-hand side.
  /// @param rhs Operator right-hand side.
  /// @return Result of division.
  friend Variable<Scalar> operator/(const Variable<Scalar>& lhs,
                                    const Variable<Scalar>& rhs) {
    return Variable{lhs.expr / rhs.expr};
  }

  /// Variable-Variable compound division operator.
  ///
  /// @param rhs Operator right-hand side.
  /// @return Result of division.
  Variable<Scalar>& operator/=(const Variable<Scalar>& rhs) {
    *this = *this / rhs;
    return *this;
  }

  /// Variable-Variable addition operator.
  ///
  /// @param lhs Operator left-hand side.
  /// @param rhs Operator right-hand side.
  /// @return Result of addition.
  friend Variable<Scalar> operator+(const Variable<Scalar>& lhs,
                                    const Variable<Scalar>& rhs) {
    return Variable{lhs.expr + rhs.expr};
  }

  /// Variable-Variable compound addition operator.
  ///
  /// @param rhs Operator right-hand side.
  /// @return Result of addition.
  Variable<Scalar>& operator+=(const Variable<Scalar>& rhs) {
    *this = *this + rhs;
    return *this;
  }

  /// Variable-Variable subtraction operator.
  ///
  /// @param lhs Operator left-hand side.
  /// @param rhs Operator right-hand side.
  /// @return Result of subtraction.
  friend Variable<Scalar> operator-(const Variable<Scalar>& lhs,
                                    const Variable<Scalar>& rhs) {
    return Variable{lhs.expr - rhs.expr};
  }

  /// Variable-Variable compound subtraction operator.
  ///
  /// @param rhs Operator right-hand side.
  /// @return Result of subtraction.
  Variable<Scalar>& operator-=(const Variable<Scalar>& rhs) {
    *this = *this - rhs;
    return *this;
  }

  /// Unary minus operator.
  ///
  /// @param lhs Operand for unary minus.
  friend Variable<Scalar> operator-(const Variable<Scalar>& lhs) {
    return Variable{-lhs.expr};
  }

  /// Unary plus operator.
  ///
  /// @param lhs Operand for unary plus.
  friend Variable<Scalar> operator+(const Variable<Scalar>& lhs) {
    return Variable{+lhs.expr};
  }

 private:
  /// The expression node
  detail::ExpressionPtr<Scalar> expr =
      detail::make_expression_ptr<detail::DecisionVariableExpression<Scalar>>();

  /// Used to update the value of this variable based on the values of its
  /// dependent variables
  gch::small_vector<detail::Expression<Scalar>*> m_graph;

  /// Used for lazy initialization of m_graph
  bool m_graph_initialized = false;

  template <typename Scalar>
  friend Variable<Scalar> abs(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> acos(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> asin(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> atan(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> atan2(const ScalarLike auto& y,
                                const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> atan2(const Variable<Scalar>& y,
                                const ScalarLike auto& x);
  template <typename Scalar>
  friend Variable<Scalar> atan2(const Variable<Scalar>& y,
                                const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> cbrt(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> cos(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> cosh(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> erf(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> exp(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> hypot(const ScalarLike auto& x,
                                const Variable<Scalar>& y);
  template <typename Scalar>
  friend Variable<Scalar> hypot(const Variable<Scalar>& x,
                                const ScalarLike auto& y);
  template <typename Scalar>
  friend Variable<Scalar> hypot(const Variable<Scalar>& x,
                                const Variable<Scalar>& y);
  template <typename Scalar>
  friend Variable<Scalar> log(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> log10(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> pow(const ScalarLike auto& base,
                              const Variable<Scalar>& power);
  template <typename Scalar>
  friend Variable<Scalar> pow(const Variable<Scalar>& base,
                              const ScalarLike auto& power);
  template <typename Scalar>
  friend Variable<Scalar> pow(const Variable<Scalar>& base,
                              const Variable<Scalar>& power);
  template <typename Scalar>
  friend Variable<Scalar> sign(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> sin(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> sinh(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> sqrt(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> tan(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> tanh(const Variable<Scalar>& x);
  template <typename Scalar>
  friend Variable<Scalar> hypot(const Variable<Scalar>& x,
                                const Variable<Scalar>& y,
                                const Variable<Scalar>& z);

  friend class detail::GradientExpressionGraph<Scalar>;
  template <typename Scalar, int UpLo>
    requires(UpLo == Eigen::Lower) || (UpLo == (Eigen::Lower | Eigen::Upper))
  friend class Hessian;
  template <typename Scalar>
  friend class Jacobian;
};

template <template <typename> typename T, typename Scalar>
  requires SleipnirMatrixLike<T<Scalar>, Scalar>
Variable(T<Scalar>) -> Variable<Scalar>;

template <std::floating_point T>
Variable(T) -> Variable<T>;

template <std::integral T>
Variable(T) -> Variable<T>;

/// abs() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> abs(const Variable<Scalar>& x) {
  return Variable{detail::abs(x.expr)};
}

/// acos() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> acos(const Variable<Scalar>& x) {
  return Variable{detail::acos(x.expr)};
}

/// asin() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> asin(const Variable<Scalar>& x) {
  return Variable{detail::asin(x.expr)};
}

/// atan() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> atan(const Variable<Scalar>& x) {
  return Variable{detail::atan(x.expr)};
}

/// atan2() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param y The y argument.
/// @param x The x argument.
template <typename Scalar>
Variable<Scalar> atan2(const ScalarLike auto& y, const Variable<Scalar>& x) {
  return Variable{detail::atan2(Variable<Scalar>(y).expr, x.expr)};
}

/// atan2() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param y The y argument.
/// @param x The x argument.
template <typename Scalar>
Variable<Scalar> atan2(const Variable<Scalar>& y, const ScalarLike auto& x) {
  return Variable{detail::atan2(y.expr, Variable<Scalar>(x).expr)};
}

/// atan2() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param y The y argument.
/// @param x The x argument.
template <typename Scalar>
Variable<Scalar> atan2(const Variable<Scalar>& y, const Variable<Scalar>& x) {
  return Variable{detail::atan2(y.expr, x.expr)};
}

/// cbrt() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> cbrt(const Variable<Scalar>& x) {
  return Variable{detail::cbrt(x.expr)};
}

/// cos() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> cos(const Variable<Scalar>& x) {
  return Variable{detail::cos(x.expr)};
}

/// cosh() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> cosh(const Variable<Scalar>& x) {
  return Variable{detail::cosh(x.expr)};
}

/// erf() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> erf(const Variable<Scalar>& x) {
  return Variable{detail::erf(x.expr)};
}

/// exp() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> exp(const Variable<Scalar>& x) {
  return Variable{detail::exp(x.expr)};
}

/// hypot() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The x argument.
/// @param y The y argument.
template <typename Scalar>
Variable<Scalar> hypot(const ScalarLike auto& x, const Variable<Scalar>& y) {
  return Variable{detail::hypot(Variable<Scalar>(x).expr, y.expr)};
}

/// hypot() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The x argument.
/// @param y The y argument.
template <typename Scalar>
Variable<Scalar> hypot(const Variable<Scalar>& x, const ScalarLike auto& y) {
  return Variable{detail::hypot(x.expr, Variable<Scalar>(y).expr)};
}

/// hypot() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The x argument.
/// @param y The y argument.
template <typename Scalar>
Variable<Scalar> hypot(const Variable<Scalar>& x, const Variable<Scalar>& y) {
  return Variable{detail::hypot(x.expr, y.expr)};
}

/// log() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> log(const Variable<Scalar>& x) {
  return Variable{detail::log(x.expr)};
}

/// log10() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> log10(const Variable<Scalar>& x) {
  return Variable{detail::log10(x.expr)};
}

/// pow() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param base The base.
/// @param power The power.
template <typename Scalar>
Variable<Scalar> pow(const ScalarLike auto& base,
                     const Variable<Scalar>& power) {
  return Variable{detail::pow(Variable<Scalar>(base).expr, power.expr)};
}

/// pow() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param base The base.
/// @param power The power.
template <typename Scalar>
Variable<Scalar> pow(const Variable<Scalar>& base,
                     const ScalarLike auto& power) {
  return Variable{detail::pow(base.expr, Variable<Scalar>(power).expr)};
}

/// pow() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param base The base.
/// @param power The power.
template <typename Scalar>
Variable<Scalar> pow(const Variable<Scalar>& base,
                     const Variable<Scalar>& power) {
  return Variable{detail::pow(base.expr, power.expr)};
}

/// sign() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> sign(const Variable<Scalar>& x) {
  return Variable{detail::sign(x.expr)};
}

/// sin() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> sin(const Variable<Scalar>& x) {
  return Variable{detail::sin(x.expr)};
}

/// sinh() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> sinh(const Variable<Scalar>& x) {
  return Variable{detail::sinh(x.expr)};
}

/// sqrt() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> sqrt(const Variable<Scalar>& x) {
  return Variable{detail::sqrt(x.expr)};
}

/// tan() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> tan(const Variable<Scalar>& x) {
  return Variable{detail::tan(x.expr)};
}

/// tanh() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The argument.
template <typename Scalar>
Variable<Scalar> tanh(const Variable<Scalar>& x) {
  return Variable{detail::tanh(x.expr)};
}

/// hypot() for Variables.
///
/// @tparam Scalar Scalar type.
/// @param x The x argument.
/// @param y The y argument.
/// @param z The z argument.
template <typename Scalar>
Variable<Scalar> hypot(const Variable<Scalar>& x, const Variable<Scalar>& y,
                       const Variable<Scalar>& z) {
  return Variable{sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2))};
}

// The standard form for equality constraints is c(x) = 0, and the standard form
// for inequality constraints is c(x) ≥ 0. make_constraints() takes constraints
// of the form lhs = rhs or lhs ≥ rhs and converts them to lhs - rhs = 0 or
// lhs - rhs ≥ 0.

template <typename Scalar, ScalarLike LHS, ScalarLike RHS>
  requires SleipnirScalarLike<LHS, Scalar> || SleipnirScalarLike<RHS, Scalar>
auto make_constraints(LHS&& lhs, RHS&& rhs) {
  gch::small_vector<Variable<Scalar>> constraints;
  constraints.emplace_back(lhs - rhs);

  return constraints;
}

template <typename Scalar, ScalarLike LHS, MatrixLike RHS>
  requires SleipnirScalarLike<LHS, Scalar> || SleipnirMatrixLike<RHS, Scalar>
auto make_constraints(LHS&& lhs, RHS&& rhs) {
  gch::small_vector<Variable<Scalar>> constraints;
  constraints.reserve(rhs.rows() * rhs.cols());

  for (int row = 0; row < rhs.rows(); ++row) {
    for (int col = 0; col < rhs.cols(); ++col) {
      // Make right-hand side zero
      constraints.emplace_back(lhs - rhs(row, col));
    }
  }

  return constraints;
}

template <typename Scalar, MatrixLike LHS, ScalarLike RHS>
  requires SleipnirMatrixLike<LHS, Scalar> || SleipnirScalarLike<RHS, Scalar>
auto make_constraints(LHS&& lhs, RHS&& rhs) {
  gch::small_vector<Variable<Scalar>> constraints;
  constraints.reserve(lhs.rows() * lhs.cols());

  for (int row = 0; row < lhs.rows(); ++row) {
    for (int col = 0; col < lhs.cols(); ++col) {
      // Make right-hand side zero
      constraints.emplace_back(lhs(row, col) - rhs);
    }
  }

  return constraints;
}

template <typename Scalar, MatrixLike LHS, MatrixLike RHS>
  requires SleipnirMatrixLike<LHS, Scalar> || SleipnirMatrixLike<RHS, Scalar>
auto make_constraints(LHS&& lhs, RHS&& rhs) {
  slp_assert(lhs.rows() == rhs.rows() && lhs.cols() == rhs.cols());

  gch::small_vector<Variable<Scalar>> constraints;
  constraints.reserve(lhs.rows() * lhs.cols());

  for (int row = 0; row < lhs.rows(); ++row) {
    for (int col = 0; col < lhs.cols(); ++col) {
      // Make right-hand side zero
      constraints.emplace_back(lhs(row, col) - rhs(row, col));
    }
  }

  return constraints;
}

/// A vector of equality constraints of the form cₑ(x) = 0.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
struct EqualityConstraints {
  /// A vector of scalar equality constraints.
  gch::small_vector<Variable<Scalar>> constraints;

  /// Concatenates multiple equality constraints.
  ///
  /// @param equality_constraints The list of EqualityConstraints to
  ///     concatenate.
  EqualityConstraints(
      std::initializer_list<EqualityConstraints> equality_constraints) {
    for (const auto& elem : equality_constraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /// Concatenates multiple equality constraints.
  ///
  /// This overload is for Python bindings only.
  ///
  /// @param equality_constraints The list of EqualityConstraints to
  ///     concatenate.
  explicit EqualityConstraints(
      const std::vector<EqualityConstraints>& equality_constraints) {
    for (const auto& elem : equality_constraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /// Constructs an equality constraint from a left and right side.
  ///
  /// The standard form for equality constraints is c(x) = 0. This function
  /// takes a constraint of the form lhs = rhs and converts it to lhs - rhs = 0.
  ///
  /// @param lhs Left-hand side.
  /// @param rhs Right-hand side.
  template <typename LHS, typename RHS>
    requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
            (ScalarLike<RHS> || MatrixLike<RHS>) &&
            (SleipnirType<LHS> || SleipnirType<RHS>)
  EqualityConstraints(LHS&& lhs, RHS&& rhs)
      : constraints{make_constraints<Scalar>(lhs, rhs)} {}

  /// Implicit conversion operator to bool.
  // NOLINTNEXTLINE (google-explicit-constructor)
  operator bool() {
    return std::ranges::all_of(constraints, [](auto& constraint) {
      return constraint.value() == Scalar(0);
    });
  }
};

/// A vector of inequality constraints of the form cᵢ(x) ≥ 0.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
struct InequalityConstraints {
  /// A vector of scalar inequality constraints.
  gch::small_vector<Variable<Scalar>> constraints;

  /// Concatenates multiple inequality constraints.
  ///
  /// @param inequality_constraints The list of InequalityConstraints to
  ///     concatenate.
  InequalityConstraints(  // NOLINT
      std::initializer_list<InequalityConstraints> inequality_constraints) {
    for (const auto& elem : inequality_constraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /// Concatenates multiple inequality constraints.
  ///
  /// This overload is for Python bindings only.
  ///
  /// @param inequality_constraints The list of InequalityConstraints to
  ///     concatenate.
  explicit InequalityConstraints(
      const std::vector<InequalityConstraints>& inequality_constraints) {
    for (const auto& elem : inequality_constraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /// Constructs an inequality constraint from a left and right side.
  ///
  /// The standard form for inequality constraints is c(x) ≥ 0. This function
  /// takes a constraints of the form lhs ≥ rhs and converts it to lhs - rhs ≥
  /// 0.
  ///
  /// @param lhs Left-hand side.
  /// @param rhs Right-hand side.
  template <typename LHS, typename RHS>
    requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
            (ScalarLike<RHS> || MatrixLike<RHS>) &&
            (SleipnirType<LHS> || SleipnirType<RHS>)
  InequalityConstraints(LHS&& lhs, RHS&& rhs)
      : constraints{make_constraints<Scalar>(lhs, rhs)} {}

  /// Implicit conversion operator to bool.
  // NOLINTNEXTLINE (google-explicit-constructor)
  operator bool() {
    return std::ranges::all_of(constraints, [](auto& constraint) {
      return constraint.value() >= Scalar(0);
    });
  }
};

/// Equality operator that returns an equality constraint for two Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) && SleipnirType<LHS> &&
          (ScalarLike<RHS> || MatrixLike<RHS>) && (!SleipnirType<RHS>)
auto operator==(LHS&& lhs, RHS&& rhs) {
  return EqualityConstraints<typename std::decay_t<LHS>::Scalar>{lhs, rhs};
}

/// Equality operator that returns an equality constraint for two Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) && (!SleipnirType<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) && SleipnirType<RHS>
auto operator==(LHS&& lhs, RHS&& rhs) {
  return EqualityConstraints<typename std::decay_t<RHS>::Scalar>{lhs, rhs};
}

/// Equality operator that returns an equality constraint for two Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) && SleipnirType<LHS> &&
          (ScalarLike<RHS> || MatrixLike<RHS>) && SleipnirType<RHS>
auto operator==(LHS&& lhs, RHS&& rhs) {
  return EqualityConstraints<typename std::decay_t<LHS>::Scalar>{lhs, rhs};
}

/// Less-than comparison operator that returns an inequality constraint for two
/// Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
auto operator<(LHS&& lhs, RHS&& rhs) {
  return rhs >= lhs;
}

/// Less-than-or-equal-to comparison operator that returns an inequality
/// constraint for two Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
auto operator<=(LHS&& lhs, RHS&& rhs) {
  return rhs >= lhs;
}

/// Greater-than comparison operator that returns an inequality constraint for
/// two Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) &&
          (SleipnirType<LHS> || SleipnirType<RHS>)
auto operator>(LHS&& lhs, RHS&& rhs) {
  return lhs >= rhs;
}

/// Greater-than-or-equal-to comparison operator that returns an inequality
/// constraint for two Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) && SleipnirType<LHS> &&
          (ScalarLike<RHS> || MatrixLike<RHS>) && (!SleipnirType<RHS>)
auto operator>=(LHS&& lhs, RHS&& rhs) {
  return InequalityConstraints<typename std::decay_t<LHS>::Scalar>{lhs, rhs};
}

/// Greater-than-or-equal-to comparison operator that returns an inequality
/// constraint for two Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) && (!SleipnirType<LHS>) &&
          (ScalarLike<RHS> || MatrixLike<RHS>) && SleipnirType<RHS>
auto operator>=(LHS&& lhs, RHS&& rhs) {
  return InequalityConstraints<typename std::decay_t<RHS>::Scalar>{lhs, rhs};
}

/// Greater-than-or-equal-to comparison operator that returns an inequality
/// constraint for two Variables.
///
/// @param lhs Left-hand side.
/// @param rhs Left-hand side.
template <typename LHS, typename RHS>
  requires(ScalarLike<LHS> || MatrixLike<LHS>) && SleipnirType<LHS> &&
          (ScalarLike<RHS> || MatrixLike<RHS>) && SleipnirType<RHS>
auto operator>=(LHS&& lhs, RHS&& rhs) {
  return InequalityConstraints<typename std::decay_t<LHS>::Scalar>{lhs, rhs};
}

/// Helper function for creating bound constraints.
///
/// @param l Lower bound.
/// @param x Variable to bound.
/// @param u Upper bound.
template <typename L, typename X, typename U>
  requires(ScalarLike<L> || MatrixLike<L>) && SleipnirType<X> &&
          (ScalarLike<U> || MatrixLike<U>)
auto bounds(L&& l, X&& x, U&& u) {
  return InequalityConstraints{l <= x, x <= u};
}

}  // namespace slp

namespace Eigen {

/// NumTraits specialization that allows instantiating Eigen types with
/// Variable.
///
/// @tparam Scalar Scalar type.
template <typename Scalar>
struct NumTraits<slp::Variable<Scalar>> : NumTraits<Scalar> {
  /// Real type.
  using Real = slp::Variable<Scalar>;
  /// Non-integer type.
  using NonInteger = slp::Variable<Scalar>;
  /// Nested type.
  using Nested = slp::Variable<Scalar>;

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
