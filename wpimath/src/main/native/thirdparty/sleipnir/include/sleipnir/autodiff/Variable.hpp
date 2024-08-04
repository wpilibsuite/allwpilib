// Copyright (c) Sleipnir contributors

#pragma once

#include <algorithm>
#include <concepts>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <wpi/SmallVector.h>

#include "sleipnir/autodiff/Expression.hpp"
#include "sleipnir/autodiff/ExpressionGraph.hpp"
#include "sleipnir/util/Assert.hpp"
#include "sleipnir/util/Concepts.hpp"
#include "sleipnir/util/Print.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir {

// Forward declarations for friend declarations in Variable
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
   * Constructs a Variable from a double.
   *
   * @param value The value of the Variable.
   */
  Variable(double value) : expr{detail::MakeExpressionPtr(value)} {}  // NOLINT

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
   */
  Variable& operator=(double value) {
    expr = detail::MakeExpressionPtr(value);

    return *this;
  }

  /**
   * Sets Variable's internal value.
   *
   * @param value The value of the Variable.
   */
  void SetValue(double value) {
    if (expr->IsConstant(0.0)) {
      expr = detail::MakeExpressionPtr(value);
    } else {
      // We only need to check the first argument since unary and binary
      // operators both use it
      if (expr->args[0] != nullptr && !expr->args[0]->IsConstant(0.0)) {
        sleipnir::println(
            stderr,
            "WARNING: {}:{}: Modified the value of a dependent variable",
            __FILE__, __LINE__);
      }
      expr->value = value;
    }
  }

  /**
   * Variable-Variable multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT Variable operator*(const Variable& lhs,
                                               const Variable& rhs) {
    return Variable{lhs.expr * rhs.expr};
  }

  /**
   * Variable-Variable compound multiplication operator.
   *
   * @param rhs Operator right-hand side.
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
   */
  friend SLEIPNIR_DLLEXPORT Variable operator/(const Variable& lhs,
                                               const Variable& rhs) {
    return Variable{lhs.expr / rhs.expr};
  }

  /**
   * Variable-Variable compound division operator.
   *
   * @param rhs Operator right-hand side.
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
   */
  friend SLEIPNIR_DLLEXPORT Variable operator+(const Variable& lhs,
                                               const Variable& rhs) {
    return Variable{lhs.expr + rhs.expr};
  }

  /**
   * Variable-Variable compound addition operator.
   *
   * @param rhs Operator right-hand side.
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
   */
  friend SLEIPNIR_DLLEXPORT Variable operator-(const Variable& lhs,
                                               const Variable& rhs) {
    return Variable{lhs.expr - rhs.expr};
  }

  /**
   * Variable-Variable compound subtraction operator.
   *
   * @param rhs Operator right-hand side.
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
   */
  double Value() {
    // Updates the value of this variable based on the values of its dependent
    // variables
    detail::ExpressionGraph{expr}.Update();

    return expr->value;
  }

  /**
   * Returns the type of this expression (constant, linear, quadratic, or
   * nonlinear).
   */
  ExpressionType Type() const { return expr->type; }

 private:
  /// The expression node.
  detail::ExpressionPtr expr =
      detail::MakeExpressionPtr(0.0, ExpressionType::kLinear);

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
  return Variable{sleipnir::sqrt(sleipnir::pow(x, 2) + sleipnir::pow(y, 2) +
                                 sleipnir::pow(z, 2))};
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
  requires(ScalarLike<std::decay_t<LHS>> || MatrixLike<std::decay_t<LHS>>) &&
          (ScalarLike<std::decay_t<RHS>> || MatrixLike<std::decay_t<RHS>>) &&
          (!std::same_as<std::decay_t<LHS>, double> ||
           !std::same_as<std::decay_t<RHS>, double>)
wpi::SmallVector<Variable> MakeConstraints(LHS&& lhs, RHS&& rhs) {
  wpi::SmallVector<Variable> constraints;

  if constexpr (ScalarLike<std::decay_t<LHS>> &&
                ScalarLike<std::decay_t<RHS>>) {
    constraints.emplace_back(lhs - rhs);
  } else if constexpr (ScalarLike<std::decay_t<LHS>> &&
                       MatrixLike<std::decay_t<RHS>>) {
    int rows;
    int cols;
    if constexpr (EigenMatrixLike<std::decay_t<RHS>>) {
      rows = rhs.rows();
      cols = rhs.cols();
    } else {
      rows = rhs.Rows();
      cols = rhs.Cols();
    }

    constraints.reserve(rows * cols);

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        // Make right-hand side zero
        constraints.emplace_back(lhs - rhs(row, col));
      }
    }
  } else if constexpr (MatrixLike<std::decay_t<LHS>> &&
                       ScalarLike<std::decay_t<RHS>>) {
    int rows;
    int cols;
    if constexpr (EigenMatrixLike<std::decay_t<LHS>>) {
      rows = lhs.rows();
      cols = lhs.cols();
    } else {
      rows = lhs.Rows();
      cols = lhs.Cols();
    }

    constraints.reserve(rows * cols);

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        // Make right-hand side zero
        constraints.emplace_back(lhs(row, col) - rhs);
      }
    }
  } else if constexpr (MatrixLike<std::decay_t<LHS>> &&
                       MatrixLike<std::decay_t<RHS>>) {
    int lhsRows;
    int lhsCols;
    if constexpr (EigenMatrixLike<std::decay_t<LHS>>) {
      lhsRows = lhs.rows();
      lhsCols = lhs.cols();
    } else {
      lhsRows = lhs.Rows();
      lhsCols = lhs.Cols();
    }

    [[maybe_unused]]
    int rhsRows;
    [[maybe_unused]]
    int rhsCols;
    if constexpr (EigenMatrixLike<std::decay_t<RHS>>) {
      rhsRows = rhs.rows();
      rhsCols = rhs.cols();
    } else {
      rhsRows = rhs.Rows();
      rhsCols = rhs.Cols();
    }

    Assert(lhsRows == rhsRows && lhsCols == rhsCols);
    constraints.reserve(lhsRows * lhsCols);

    for (int row = 0; row < lhsRows; ++row) {
      for (int col = 0; col < lhsCols; ++col) {
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
  wpi::SmallVector<Variable> constraints;

  /**
   * Concatenates multiple equality constraints.
   *
   * @param equalityConstraints The list of EqualityConstraints to concatenate.
   */
  EqualityConstraints(  // NOLINT
      std::initializer_list<EqualityConstraints> equalityConstraints) {
    for (const auto& elem : equalityConstraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /**
   * Concatenates multiple equality constraints.
   *
   * This overload is for Python bindings only.
   *
   * @param equalityConstraints The list of EqualityConstraints to concatenate.
   */
  explicit EqualityConstraints(
      const std::vector<EqualityConstraints>& equalityConstraints) {
    for (const auto& elem : equalityConstraints) {
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
    requires(ScalarLike<std::decay_t<LHS>> || MatrixLike<std::decay_t<LHS>>) &&
            (ScalarLike<std::decay_t<RHS>> || MatrixLike<std::decay_t<RHS>>) &&
            (!std::same_as<std::decay_t<LHS>, double> ||
             !std::same_as<std::decay_t<RHS>, double>)
  EqualityConstraints(LHS&& lhs, RHS&& rhs)
      : constraints{MakeConstraints(lhs, rhs)} {}

  /**
   * Implicit conversion operator to bool.
   */
  operator bool() {  // NOLINT
    return std::all_of(
        constraints.begin(), constraints.end(),
        [](auto& constraint) { return constraint.Value() == 0.0; });
  }
};

/**
 * A vector of inequality constraints of the form cᵢ(x) ≥ 0.
 */
struct SLEIPNIR_DLLEXPORT InequalityConstraints {
  /// A vector of scalar inequality constraints.
  wpi::SmallVector<Variable> constraints;

  /**
   * Concatenates multiple inequality constraints.
   *
   * @param inequalityConstraints The list of InequalityConstraints to
   * concatenate.
   */
  InequalityConstraints(  // NOLINT
      std::initializer_list<InequalityConstraints> inequalityConstraints) {
    for (const auto& elem : inequalityConstraints) {
      constraints.insert(constraints.end(), elem.constraints.begin(),
                         elem.constraints.end());
    }
  }

  /**
   * Concatenates multiple inequality constraints.
   *
   * This overload is for Python bindings only.
   *
   * @param inequalityConstraints The list of InequalityConstraints to
   * concatenate.
   */
  explicit InequalityConstraints(
      const std::vector<InequalityConstraints>& inequalityConstraints) {
    for (const auto& elem : inequalityConstraints) {
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
    requires(ScalarLike<std::decay_t<LHS>> || MatrixLike<std::decay_t<LHS>>) &&
            (ScalarLike<std::decay_t<RHS>> || MatrixLike<std::decay_t<RHS>>) &&
            (!std::same_as<std::decay_t<LHS>, double> ||
             !std::same_as<std::decay_t<RHS>, double>)
  InequalityConstraints(LHS&& lhs, RHS&& rhs)
      : constraints{MakeConstraints(lhs, rhs)} {}

  /**
   * Implicit conversion operator to bool.
   */
  operator bool() {  // NOLINT
    return std::all_of(
        constraints.begin(), constraints.end(),
        [](auto& constraint) { return constraint.Value() >= 0.0; });
  }
};

/**
 * Equality operator that returns an equality constraint for two Variables.
 *
 * @param lhs Left-hand side.
 * @param rhs Left-hand side.
 */
template <typename LHS, typename RHS>
  requires(ScalarLike<std::decay_t<LHS>> || MatrixLike<std::decay_t<LHS>>) &&
          (ScalarLike<std::decay_t<RHS>> || MatrixLike<std::decay_t<RHS>>) &&
          (!std::same_as<std::decay_t<LHS>, double> ||
           !std::same_as<std::decay_t<RHS>, double>)
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
  requires(ScalarLike<std::decay_t<LHS>> || MatrixLike<std::decay_t<LHS>>) &&
          (ScalarLike<std::decay_t<RHS>> || MatrixLike<std::decay_t<RHS>>) &&
          (!std::same_as<std::decay_t<LHS>, double> ||
           !std::same_as<std::decay_t<RHS>, double>)
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
  requires(ScalarLike<std::decay_t<LHS>> || MatrixLike<std::decay_t<LHS>>) &&
          (ScalarLike<std::decay_t<RHS>> || MatrixLike<std::decay_t<RHS>>) &&
          (!std::same_as<std::decay_t<LHS>, double> ||
           !std::same_as<std::decay_t<RHS>, double>)
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
  requires(ScalarLike<std::decay_t<LHS>> || MatrixLike<std::decay_t<LHS>>) &&
          (ScalarLike<std::decay_t<RHS>> || MatrixLike<std::decay_t<RHS>>) &&
          (!std::same_as<std::decay_t<LHS>, double> ||
           !std::same_as<std::decay_t<RHS>, double>)
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
  requires(ScalarLike<std::decay_t<LHS>> || MatrixLike<std::decay_t<LHS>>) &&
          (ScalarLike<std::decay_t<RHS>> || MatrixLike<std::decay_t<RHS>>) &&
          (!std::same_as<std::decay_t<LHS>, double> ||
           !std::same_as<std::decay_t<RHS>, double>)
InequalityConstraints operator>=(LHS&& lhs, RHS&& rhs) {
  return InequalityConstraints{lhs, rhs};
}

}  // namespace sleipnir

namespace Eigen {

/**
 * NumTraits specialization that allows instantiating Eigen types with Variable.
 */
template <>
struct NumTraits<sleipnir::Variable> : NumTraits<double> {
  using Real = sleipnir::Variable;
  using NonInteger = sleipnir::Variable;
  using Nested = sleipnir::Variable;

  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };
};

}  // namespace Eigen
