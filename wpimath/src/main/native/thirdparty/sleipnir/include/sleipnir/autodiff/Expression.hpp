// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <numbers>
#include <utility>

#include <wpi/SmallVector.h>

#include "sleipnir/autodiff/ExpressionType.hpp"
#include "sleipnir/util/IntrusiveSharedPtr.hpp"
#include "sleipnir/util/Pool.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir::detail {

// The global pool allocator uses a thread-local static pool resource, which
// isn't guaranteed to be initialized properly across DLL boundaries on Windows
#ifdef _WIN32
inline constexpr bool kUsePoolAllocator = false;
#else
inline constexpr bool kUsePoolAllocator = true;
#endif

struct SLEIPNIR_DLLEXPORT Expression;

inline void IntrusiveSharedPtrIncRefCount(Expression* expr);
inline void IntrusiveSharedPtrDecRefCount(Expression* expr);

/**
 * Typedef for intrusive shared pointer to Expression.
 */
using ExpressionPtr = IntrusiveSharedPtr<Expression>;

/**
 * Creates an intrusive shared pointer to an expression from the global pool
 * allocator.
 *
 * @param args Constructor arguments for Expression.
 */
template <typename... Args>
static ExpressionPtr MakeExpressionPtr(Args&&... args) {
  if constexpr (kUsePoolAllocator) {
    return AllocateIntrusiveShared<Expression>(
        GlobalPoolAllocator<Expression>(), std::forward<Args>(args)...);
  } else {
    return MakeIntrusiveShared<Expression>(std::forward<Args>(args)...);
  }
}

/**
 * An autodiff expression node.
 */
struct SLEIPNIR_DLLEXPORT Expression {
  /**
   * Binary function taking two doubles and returning a double.
   */
  using BinaryFuncDouble = double (*)(double, double);

  /**
   * Trinary function taking three doubles and returning a double.
   */
  using TrinaryFuncDouble = double (*)(double, double, double);

  /**
   * Trinary function taking three expressions and returning an expression.
   */
  using TrinaryFuncExpr = ExpressionPtr (*)(const ExpressionPtr&,
                                            const ExpressionPtr&,
                                            const ExpressionPtr&);

  /// The value of the expression node.
  double value = 0.0;

  /// The adjoint of the expression node used during autodiff.
  double adjoint = 0.0;

  /// Tracks the number of instances of this expression yet to be encountered in
  /// an expression tree.
  uint32_t duplications = 0;

  /// This expression's row in wrt for autodiff gradient, Jacobian, or Hessian.
  /// This is -1 if the expression isn't in wrt.
  int32_t row = -1;

  /// The adjoint of the expression node used during gradient expression tree
  /// generation.
  ExpressionPtr adjointExpr;

  /// Expression argument type.
  ExpressionType type = ExpressionType::kConstant;

  /// Reference count for intrusive shared pointer.
  uint32_t refCount = 0;

  /// Either nullary operator with no arguments, unary operator with one
  /// argument, or binary operator with two arguments. This operator is
  /// used to update the node's value.
  BinaryFuncDouble valueFunc = nullptr;

  /// Functions returning double adjoints of the children expressions.
  ///
  /// Parameters:
  /// <ul>
  ///   <li>lhs: Left argument to binary operator.</li>
  ///   <li>rhs: Right argument to binary operator.</li>
  ///   <li>parentAdjoint: Adjoint of parent expression.</li>
  /// </ul>
  std::array<TrinaryFuncDouble, 2> gradientValueFuncs{nullptr, nullptr};

  /// Functions returning Variable adjoints of the children expressions.
  ///
  /// Parameters:
  /// <ul>
  ///   <li>lhs: Left argument to binary operator.</li>
  ///   <li>rhs: Right argument to binary operator.</li>
  ///   <li>parentAdjoint: Adjoint of parent expression.</li>
  /// </ul>
  std::array<TrinaryFuncExpr, 2> gradientFuncs{nullptr, nullptr};

  /// Expression arguments.
  std::array<ExpressionPtr, 2> args{nullptr, nullptr};

  /**
   * Constructs a constant expression with a value of zero.
   */
  constexpr Expression() = default;

  /**
   * Constructs a nullary expression (an operator with no arguments).
   *
   * @param value The expression value.
   * @param type The expression type. It should be either constant (the default)
   *             or linear.
   */
  explicit constexpr Expression(double value,
                                ExpressionType type = ExpressionType::kConstant)
      : value{value}, type{type} {}

  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param type The expression's type.
   * @param valueFunc Unary operator that produces this expression's value.
   * @param lhsGradientValueFunc Gradient with respect to the operand.
   * @param lhsGradientFunc Gradient with respect to the operand.
   * @param lhs Unary operator's operand.
   */
  constexpr Expression(ExpressionType type, BinaryFuncDouble valueFunc,
                       TrinaryFuncDouble lhsGradientValueFunc,
                       TrinaryFuncExpr lhsGradientFunc, ExpressionPtr lhs)
      : value{valueFunc(lhs->value, 0.0)},
        type{type},
        valueFunc{valueFunc},
        gradientValueFuncs{lhsGradientValueFunc, nullptr},
        gradientFuncs{lhsGradientFunc, nullptr},
        args{lhs, nullptr} {}

  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param type The expression's type.
   * @param valueFunc Unary operator that produces this expression's value.
   * @param lhsGradientValueFunc Gradient with respect to the left operand.
   * @param rhsGradientValueFunc Gradient with respect to the right operand.
   * @param lhsGradientFunc Gradient with respect to the left operand.
   * @param rhsGradientFunc Gradient with respect to the right operand.
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr Expression(ExpressionType type, BinaryFuncDouble valueFunc,
                       TrinaryFuncDouble lhsGradientValueFunc,
                       TrinaryFuncDouble rhsGradientValueFunc,
                       TrinaryFuncExpr lhsGradientFunc,
                       TrinaryFuncExpr rhsGradientFunc, ExpressionPtr lhs,
                       ExpressionPtr rhs)
      : value{valueFunc(lhs->value, rhs->value)},
        type{type},
        valueFunc{valueFunc},
        gradientValueFuncs{lhsGradientValueFunc, rhsGradientValueFunc},
        gradientFuncs{lhsGradientFunc, rhsGradientFunc},
        args{lhs, rhs} {}

  /**
   * Returns true if the expression is the given constant.
   *
   * @param constant The constant.
   */
  constexpr bool IsConstant(double constant) const {
    return type == ExpressionType::kConstant && value == constant;
  }

  /**
   * Expression-Expression multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT ExpressionPtr operator*(const ExpressionPtr& lhs,
                                                    const ExpressionPtr& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->IsConstant(0.0)) {
      // Return zero
      return lhs;
    } else if (rhs->IsConstant(0.0)) {
      // Return zero
      return rhs;
    } else if (lhs->IsConstant(1.0)) {
      return rhs;
    } else if (rhs->IsConstant(1.0)) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type == kConstant && rhs->type == kConstant) {
      return MakeExpressionPtr(lhs->value * rhs->value);
    }

    // Evaluate expression type
    ExpressionType type;
    if (lhs->type == kConstant) {
      type = rhs->type;
    } else if (rhs->type == kConstant) {
      type = lhs->type;
    } else if (lhs->type == kLinear && rhs->type == kLinear) {
      type = kQuadratic;
    } else {
      type = kNonlinear;
    }

    return MakeExpressionPtr(
        type, [](double lhs, double rhs) { return lhs * rhs; },
        [](double, double rhs, double parentAdjoint) {
          return parentAdjoint * rhs;
        },
        [](double lhs, double, double parentAdjoint) {
          return parentAdjoint * lhs;
        },
        [](const ExpressionPtr&, const ExpressionPtr& rhs,
           const ExpressionPtr& parentAdjoint) { return parentAdjoint * rhs; },
        [](const ExpressionPtr& lhs, const ExpressionPtr&,
           const ExpressionPtr& parentAdjoint) { return parentAdjoint * lhs; },
        lhs, rhs);
  }

  /**
   * Expression-Expression division operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT ExpressionPtr operator/(const ExpressionPtr& lhs,
                                                    const ExpressionPtr& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->IsConstant(0.0)) {
      // Return zero
      return lhs;
    } else if (rhs->IsConstant(1.0)) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type == kConstant && rhs->type == kConstant) {
      return MakeExpressionPtr(lhs->value / rhs->value);
    }

    // Evaluate expression type
    ExpressionType type;
    if (rhs->type == kConstant) {
      type = lhs->type;
    } else {
      type = kNonlinear;
    }

    return MakeExpressionPtr(
        type, [](double lhs, double rhs) { return lhs / rhs; },
        [](double, double rhs, double parentAdjoint) {
          return parentAdjoint / rhs;
        },
        [](double lhs, double rhs, double parentAdjoint) {
          return parentAdjoint * -lhs / (rhs * rhs);
        },
        [](const ExpressionPtr&, const ExpressionPtr& rhs,
           const ExpressionPtr& parentAdjoint) { return parentAdjoint / rhs; },
        [](const ExpressionPtr& lhs, const ExpressionPtr& rhs,
           const ExpressionPtr& parentAdjoint) {
          return parentAdjoint * -lhs / (rhs * rhs);
        },
        lhs, rhs);
  }

  /**
   * Expression-Expression addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT ExpressionPtr operator+(const ExpressionPtr& lhs,
                                                    const ExpressionPtr& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs == nullptr || lhs->IsConstant(0.0)) {
      return rhs;
    } else if (rhs == nullptr || rhs->IsConstant(0.0)) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type == kConstant && rhs->type == kConstant) {
      return MakeExpressionPtr(lhs->value + rhs->value);
    }

    return MakeExpressionPtr(
        std::max(lhs->type, rhs->type),
        [](double lhs, double rhs) { return lhs + rhs; },
        [](double, double, double parentAdjoint) { return parentAdjoint; },
        [](double, double, double parentAdjoint) { return parentAdjoint; },
        [](const ExpressionPtr&, const ExpressionPtr&,
           const ExpressionPtr& parentAdjoint) { return parentAdjoint; },
        [](const ExpressionPtr&, const ExpressionPtr&,
           const ExpressionPtr& parentAdjoint) { return parentAdjoint; },
        lhs, rhs);
  }

  /**
   * Expression-Expression subtraction operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend SLEIPNIR_DLLEXPORT ExpressionPtr operator-(const ExpressionPtr& lhs,
                                                    const ExpressionPtr& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->IsConstant(0.0)) {
      if (rhs->IsConstant(0.0)) {
        // Return zero
        return rhs;
      } else {
        return -rhs;
      }
    } else if (rhs->IsConstant(0.0)) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type == kConstant && rhs->type == kConstant) {
      return MakeExpressionPtr(lhs->value - rhs->value);
    }

    return MakeExpressionPtr(
        std::max(lhs->type, rhs->type),
        [](double lhs, double rhs) { return lhs - rhs; },
        [](double, double, double parentAdjoint) { return parentAdjoint; },
        [](double, double, double parentAdjoint) { return -parentAdjoint; },
        [](const ExpressionPtr&, const ExpressionPtr&,
           const ExpressionPtr& parentAdjoint) { return parentAdjoint; },
        [](const ExpressionPtr&, const ExpressionPtr&,
           const ExpressionPtr& parentAdjoint) { return -parentAdjoint; },
        lhs, rhs);
  }

  /**
   * Unary minus operator.
   *
   * @param lhs Operand of unary minus.
   */
  friend SLEIPNIR_DLLEXPORT ExpressionPtr operator-(const ExpressionPtr& lhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->IsConstant(0.0)) {
      // Return zero
      return lhs;
    }

    // Evaluate constant
    if (lhs->type == kConstant) {
      return MakeExpressionPtr(-lhs->value);
    }

    return MakeExpressionPtr(
        lhs->type, [](double lhs, double) { return -lhs; },
        [](double, double, double parentAdjoint) { return -parentAdjoint; },
        [](const ExpressionPtr&, const ExpressionPtr&,
           const ExpressionPtr& parentAdjoint) { return -parentAdjoint; },
        lhs);
  }

  /**
   * Unary plus operator.
   *
   * @param lhs Operand of unary plus.
   */
  friend SLEIPNIR_DLLEXPORT ExpressionPtr operator+(const ExpressionPtr& lhs) {
    return lhs;
  }
};

SLEIPNIR_DLLEXPORT inline ExpressionPtr exp(const ExpressionPtr& x);
SLEIPNIR_DLLEXPORT inline ExpressionPtr sin(const ExpressionPtr& x);
SLEIPNIR_DLLEXPORT inline ExpressionPtr sinh(const ExpressionPtr& x);
SLEIPNIR_DLLEXPORT inline ExpressionPtr sqrt(const ExpressionPtr& x);

/**
 * Refcount increment for intrusive shared pointer.
 *
 * @param expr The shared pointer's managed object.
 */
inline void IntrusiveSharedPtrIncRefCount(Expression* expr) {
  ++expr->refCount;
}

/**
 * Refcount decrement for intrusive shared pointer.
 *
 * @param expr The shared pointer's managed object.
 */
inline void IntrusiveSharedPtrDecRefCount(Expression* expr) {
  // If a deeply nested tree is being deallocated all at once, calling the
  // Expression destructor when expr's refcount reaches zero can cause a stack
  // overflow. Instead, we iterate over its children to decrement their
  // refcounts and deallocate them.
  wpi::SmallVector<Expression*> stack;
  stack.emplace_back(expr);

  while (!stack.empty()) {
    auto elem = stack.back();
    stack.pop_back();

    // Decrement the current node's refcount. If it reaches zero, deallocate the
    // node and enqueue its children so their refcounts are decremented too.
    if (--elem->refCount == 0) {
      if (elem->adjointExpr != nullptr) {
        stack.emplace_back(elem->adjointExpr.Get());
      }
      for (auto&& arg : elem->args) {
        if (arg != nullptr) {
          stack.emplace_back(arg.Get());
        }
      }

      // Not calling the destructor here is safe because it only decrements
      // refcounts, which was already done above.
      if constexpr (kUsePoolAllocator) {
        auto alloc = GlobalPoolAllocator<Expression>();
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, elem,
                                                           sizeof(Expression));
      }
    }
  }
}

/**
 * std::abs() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr abs(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::abs(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::abs(x); },
      [](double x, double, double parentAdjoint) {
        if (x < 0.0) {
          return -parentAdjoint;
        } else if (x > 0.0) {
          return parentAdjoint;
        } else {
          return 0.0;
        }
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        if (x->value < 0.0) {
          return -parentAdjoint;
        } else if (x->value > 0.0) {
          return parentAdjoint;
        } else {
          // Return zero
          return MakeExpressionPtr();
        }
      },
      x);
}

/**
 * std::acos() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr acos(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    return MakeExpressionPtr(std::numbers::pi / 2.0);
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::acos(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::acos(x); },
      [](double x, double, double parentAdjoint) {
        return -parentAdjoint / std::sqrt(1.0 - x * x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return -parentAdjoint /
               sleipnir::detail::sqrt(MakeExpressionPtr(1.0) - x * x);
      },
      x);
}

/**
 * std::asin() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr asin(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::asin(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::asin(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint / std::sqrt(1.0 - x * x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint /
               sleipnir::detail::sqrt(MakeExpressionPtr(1.0) - x * x);
      },
      x);
}

/**
 * std::atan() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr atan(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::atan(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::atan(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint / (1.0 + x * x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint / (MakeExpressionPtr(1.0) + x * x);
      },
      x);
}

/**
 * std::atan2() for Expressions.
 *
 * @param y The y argument.
 * @param x The x argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr atan2(  // NOLINT
    const ExpressionPtr& y, const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (y->IsConstant(0.0)) {
    // Return zero
    return y;
  } else if (x->IsConstant(0.0)) {
    return MakeExpressionPtr(std::numbers::pi / 2.0);
  }

  // Evaluate constant
  if (y->type == kConstant && x->type == kConstant) {
    return MakeExpressionPtr(std::atan2(y->value, x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double y, double x) { return std::atan2(y, x); },
      [](double y, double x, double parentAdjoint) {
        return parentAdjoint * x / (y * y + x * x);
      },
      [](double y, double x, double parentAdjoint) {
        return parentAdjoint * -y / (y * y + x * x);
      },
      [](const ExpressionPtr& y, const ExpressionPtr& x,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * x / (y * y + x * x);
      },
      [](const ExpressionPtr& y, const ExpressionPtr& x,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * -y / (y * y + x * x);
      },
      y, x);
}

/**
 * std::cos() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr cos(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    return MakeExpressionPtr(1.0);
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::cos(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::cos(x); },
      [](double x, double, double parentAdjoint) {
        return -parentAdjoint * std::sin(x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * -sleipnir::detail::sin(x);
      },
      x);
}

/**
 * std::cosh() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr cosh(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    return MakeExpressionPtr(1.0);
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::cosh(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::cosh(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint * std::sinh(x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * sleipnir::detail::sinh(x);
      },
      x);
}

/**
 * std::erf() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr erf(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::erf(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::erf(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint * 2.0 * std::numbers::inv_sqrtpi *
               std::exp(-x * x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint *
               MakeExpressionPtr(2.0 * std::numbers::inv_sqrtpi) *
               sleipnir::detail::exp(-x * x);
      },
      x);
}

/**
 * std::exp() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr exp(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    return MakeExpressionPtr(1.0);
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::exp(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::exp(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint * std::exp(x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * sleipnir::detail::exp(x);
      },
      x);
}

/**
 * std::hypot() for Expressions.
 *
 * @param x The x argument.
 * @param y The y argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr hypot(  // NOLINT
    const ExpressionPtr& x, const ExpressionPtr& y) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    return y;
  } else if (y->IsConstant(0.0)) {
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant && y->type == kConstant) {
    return MakeExpressionPtr(std::hypot(x->value, y->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double y) { return std::hypot(x, y); },
      [](double x, double y, double parentAdjoint) {
        return parentAdjoint * x / std::hypot(x, y);
      },
      [](double x, double y, double parentAdjoint) {
        return parentAdjoint * y / std::hypot(x, y);
      },
      [](const ExpressionPtr& x, const ExpressionPtr& y,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * x / sleipnir::detail::hypot(x, y);
      },
      [](const ExpressionPtr& x, const ExpressionPtr& y,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * y / sleipnir::detail::hypot(x, y);
      },
      x, y);
}

/**
 * std::log() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr log(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::log(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::log(x); },
      [](double x, double, double parentAdjoint) { return parentAdjoint / x; },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) { return parentAdjoint / x; },
      x);
}

/**
 * std::log10() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr log10(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::log10(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::log10(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint / (std::numbers::ln10 * x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint / (MakeExpressionPtr(std::numbers::ln10) * x);
      },
      x);
}

/**
 * std::pow() for Expressions.
 *
 * @param base The base.
 * @param power The power.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr pow(  // NOLINT
    const ExpressionPtr& base, const ExpressionPtr& power) {
  using enum ExpressionType;

  // Prune expression
  if (base->IsConstant(0.0)) {
    // Return zero
    return base;
  } else if (base->IsConstant(1.0)) {
    return base;
  }
  if (power->IsConstant(0.0)) {
    return MakeExpressionPtr(1.0);
  } else if (power->IsConstant(1.0)) {
    return base;
  }

  // Evaluate constant
  if (base->type == kConstant && power->type == kConstant) {
    return MakeExpressionPtr(std::pow(base->value, power->value));
  }

  return MakeExpressionPtr(
      base->type == kLinear && power->IsConstant(2.0) ? kQuadratic : kNonlinear,
      [](double base, double power) { return std::pow(base, power); },
      [](double base, double power, double parentAdjoint) {
        return parentAdjoint * std::pow(base, power - 1) * power;
      },
      [](double base, double power, double parentAdjoint) {
        // Since x * std::log(x) -> 0 as x -> 0
        if (base == 0.0) {
          return 0.0;
        } else {
          return parentAdjoint * std::pow(base, power - 1) * base *
                 std::log(base);
        }
      },
      [](const ExpressionPtr& base, const ExpressionPtr& power,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint *
               sleipnir::detail::pow(base, power - MakeExpressionPtr(1.0)) *
               power;
      },
      [](const ExpressionPtr& base, const ExpressionPtr& power,
         const ExpressionPtr& parentAdjoint) {
        // Since x * std::log(x) -> 0 as x -> 0
        if (base->value == 0.0) {
          // Return zero
          return base;
        } else {
          return parentAdjoint *
                 sleipnir::detail::pow(base, power - MakeExpressionPtr(1.0)) *
                 base * sleipnir::detail::log(base);
        }
      },
      base, power);
}

/**
 * sign() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr sign(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Evaluate constant
  if (x->type == kConstant) {
    if (x->value < 0.0) {
      return MakeExpressionPtr(-1.0);
    } else if (x->value == 0.0) {
      // Return zero
      return x;
    } else {
      return MakeExpressionPtr(1.0);
    }
  }

  return MakeExpressionPtr(
      kNonlinear,
      [](double x, double) {
        if (x < 0.0) {
          return -1.0;
        } else if (x == 0.0) {
          return 0.0;
        } else {
          return 1.0;
        }
      },
      [](double, double, double) { return 0.0; },
      [](const ExpressionPtr&, const ExpressionPtr&, const ExpressionPtr&) {
        // Return zero
        return MakeExpressionPtr();
      },
      x);
}

/**
 * std::sin() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr sin(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::sin(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::sin(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint * std::cos(x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * sleipnir::detail::cos(x);
      },
      x);
}

/**
 * std::sinh() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr sinh(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::sinh(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::sinh(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint * std::cosh(x);
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint * sleipnir::detail::cosh(x);
      },
      x);
}

/**
 * std::sqrt() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr sqrt(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Evaluate constant
  if (x->type == kConstant) {
    if (x->value == 0.0) {
      // Return zero
      return x;
    } else if (x->value == 1.0) {
      return x;
    } else {
      return MakeExpressionPtr(std::sqrt(x->value));
    }
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::sqrt(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint / (2.0 * std::sqrt(x));
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint /
               (MakeExpressionPtr(2.0) * sleipnir::detail::sqrt(x));
      },
      x);
}

/**
 * std::tan() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr tan(  // NOLINT
    const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::tan(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::tan(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint / (std::cos(x) * std::cos(x));
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint /
               (sleipnir::detail::cos(x) * sleipnir::detail::cos(x));
      },
      x);
}

/**
 * std::tanh() for Expressions.
 *
 * @param x The argument.
 */
SLEIPNIR_DLLEXPORT inline ExpressionPtr tanh(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->IsConstant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type == kConstant) {
    return MakeExpressionPtr(std::tanh(x->value));
  }

  return MakeExpressionPtr(
      kNonlinear, [](double x, double) { return std::tanh(x); },
      [](double x, double, double parentAdjoint) {
        return parentAdjoint / (std::cosh(x) * std::cosh(x));
      },
      [](const ExpressionPtr& x, const ExpressionPtr&,
         const ExpressionPtr& parentAdjoint) {
        return parentAdjoint /
               (sleipnir::detail::cosh(x) * sleipnir::detail::cosh(x));
      },
      x);
}

}  // namespace sleipnir::detail
