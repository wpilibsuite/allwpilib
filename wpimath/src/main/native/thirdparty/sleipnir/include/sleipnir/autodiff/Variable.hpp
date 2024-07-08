// Copyright (c) Sleipnir contributors

#pragma once

#include <utility>

#include "sleipnir/autodiff/Expression.hpp"
#include "sleipnir/autodiff/ExpressionGraph.hpp"
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

}  // namespace sleipnir
