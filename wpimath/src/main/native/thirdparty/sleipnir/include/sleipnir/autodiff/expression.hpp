// Copyright (c) Sleipnir contributors

#pragma once

#include <stdint.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <numbers>
#include <utility>

#include <gch/small_vector.hpp>

#include "sleipnir/autodiff/expression_type.hpp"
#include "sleipnir/util/intrusive_shared_ptr.hpp"
#include "sleipnir/util/pool.hpp"

namespace slp::detail {

// The global pool allocator uses a thread-local static pool resource, which
// isn't guaranteed to be initialized properly across DLL boundaries on Windows
#ifdef _WIN32
inline constexpr bool USE_POOL_ALLOCATOR = false;
#else
inline constexpr bool USE_POOL_ALLOCATOR = true;
#endif

struct Expression;

inline constexpr void inc_ref_count(Expression* expr);
inline void dec_ref_count(Expression* expr);

/**
 * Typedef for intrusive shared pointer to Expression.
 */
using ExpressionPtr = IntrusiveSharedPtr<Expression>;

/**
 * Creates an intrusive shared pointer to an expression from the global pool
 * allocator.
 *
 * @tparam T The derived expression type.
 * @param args Constructor arguments for Expression.
 */
template <typename T, typename... Args>
static ExpressionPtr make_expression_ptr(Args&&... args) {
  if constexpr (USE_POOL_ALLOCATOR) {
    return allocate_intrusive_shared<T>(global_pool_allocator<T>(),
                                        std::forward<Args>(args)...);
  } else {
    return make_intrusive_shared<T>(std::forward<Args>(args)...);
  }
}

template <ExpressionType T>
struct BinaryMinusExpression;

template <ExpressionType T>
struct BinaryPlusExpression;

struct ConstExpression;

template <ExpressionType T>
struct DivExpression;

template <ExpressionType T>
struct MultExpression;

template <ExpressionType T>
struct UnaryMinusExpression;

/**
 * An autodiff expression node.
 */
struct Expression {
  /// The value of the expression node.
  double val = 0.0;

  /// The adjoint of the expression node used during autodiff.
  double adjoint = 0.0;

  /// Counts incoming edges for this node.
  uint32_t incoming_edges = 0;

  /// This expression's column in a Jacobian, or -1 otherwise.
  int32_t col = -1;

  /// The adjoint of the expression node used during gradient expression tree
  /// generation.
  ExpressionPtr adjoint_expr;

  /// Reference count for intrusive shared pointer.
  uint32_t ref_count = 0;

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
   */
  explicit constexpr Expression(double value) : val{value} {}

  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr Expression(ExpressionPtr lhs)
      : args{std::move(lhs), nullptr} {}

  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr Expression(ExpressionPtr lhs, ExpressionPtr rhs)
      : args{std::move(lhs), std::move(rhs)} {}

  virtual ~Expression() = default;

  /**
   * Returns true if the expression is the given constant.
   *
   * @param constant The constant.
   *
   * @return True if the expression is the given constant.
   */
  constexpr bool is_constant(double constant) const {
    return type() == ExpressionType::CONSTANT && val == constant;
  }

  /**
   * Expression-Expression multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr operator*(const ExpressionPtr& lhs,
                                 const ExpressionPtr& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->is_constant(0.0)) {
      // Return zero
      return lhs;
    } else if (rhs->is_constant(0.0)) {
      // Return zero
      return rhs;
    } else if (lhs->is_constant(1.0)) {
      return rhs;
    } else if (rhs->is_constant(1.0)) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT && rhs->type() == CONSTANT) {
      return make_expression_ptr<ConstExpression>(lhs->val * rhs->val);
    }

    // Evaluate expression type
    if (lhs->type() == CONSTANT) {
      if (rhs->type() == LINEAR) {
        return make_expression_ptr<MultExpression<LINEAR>>(lhs, rhs);
      } else if (rhs->type() == QUADRATIC) {
        return make_expression_ptr<MultExpression<QUADRATIC>>(lhs, rhs);
      } else {
        return make_expression_ptr<MultExpression<NONLINEAR>>(lhs, rhs);
      }
    } else if (rhs->type() == CONSTANT) {
      if (lhs->type() == LINEAR) {
        return make_expression_ptr<MultExpression<LINEAR>>(lhs, rhs);
      } else if (lhs->type() == QUADRATIC) {
        return make_expression_ptr<MultExpression<QUADRATIC>>(lhs, rhs);
      } else {
        return make_expression_ptr<MultExpression<NONLINEAR>>(lhs, rhs);
      }
    } else if (lhs->type() == LINEAR && rhs->type() == LINEAR) {
      return make_expression_ptr<MultExpression<QUADRATIC>>(lhs, rhs);
    } else {
      return make_expression_ptr<MultExpression<NONLINEAR>>(lhs, rhs);
    }
  }

  /**
   * Expression-Expression division operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr operator/(const ExpressionPtr& lhs,
                                 const ExpressionPtr& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->is_constant(0.0)) {
      // Return zero
      return lhs;
    } else if (rhs->is_constant(1.0)) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT && rhs->type() == CONSTANT) {
      return make_expression_ptr<ConstExpression>(lhs->val / rhs->val);
    }

    // Evaluate expression type
    if (rhs->type() == CONSTANT) {
      if (lhs->type() == LINEAR) {
        return make_expression_ptr<DivExpression<LINEAR>>(lhs, rhs);
      } else if (lhs->type() == QUADRATIC) {
        return make_expression_ptr<DivExpression<QUADRATIC>>(lhs, rhs);
      } else {
        return make_expression_ptr<DivExpression<NONLINEAR>>(lhs, rhs);
      }
    } else {
      return make_expression_ptr<DivExpression<NONLINEAR>>(lhs, rhs);
    }
  }

  /**
   * Expression-Expression addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr operator+(const ExpressionPtr& lhs,
                                 const ExpressionPtr& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs == nullptr || lhs->is_constant(0.0)) {
      return rhs;
    } else if (rhs == nullptr || rhs->is_constant(0.0)) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT && rhs->type() == CONSTANT) {
      return make_expression_ptr<ConstExpression>(lhs->val + rhs->val);
    }

    auto type = std::max(lhs->type(), rhs->type());
    if (type == LINEAR) {
      return make_expression_ptr<BinaryPlusExpression<LINEAR>>(lhs, rhs);
    } else if (type == QUADRATIC) {
      return make_expression_ptr<BinaryPlusExpression<QUADRATIC>>(lhs, rhs);
    } else {
      return make_expression_ptr<BinaryPlusExpression<NONLINEAR>>(lhs, rhs);
    }
  }

  /**
   * Expression-Expression compound addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr operator+=(ExpressionPtr& lhs,
                                  const ExpressionPtr& rhs) {
    return lhs = lhs + rhs;
  }

  /**
   * Expression-Expression subtraction operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr operator-(const ExpressionPtr& lhs,
                                 const ExpressionPtr& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->is_constant(0.0)) {
      if (rhs->is_constant(0.0)) {
        // Return zero
        return rhs;
      } else {
        return -rhs;
      }
    } else if (rhs->is_constant(0.0)) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT && rhs->type() == CONSTANT) {
      return make_expression_ptr<ConstExpression>(lhs->val - rhs->val);
    }

    auto type = std::max(lhs->type(), rhs->type());
    if (type == LINEAR) {
      return make_expression_ptr<BinaryMinusExpression<LINEAR>>(lhs, rhs);
    } else if (type == QUADRATIC) {
      return make_expression_ptr<BinaryMinusExpression<QUADRATIC>>(lhs, rhs);
    } else {
      return make_expression_ptr<BinaryMinusExpression<NONLINEAR>>(lhs, rhs);
    }
  }

  /**
   * Unary minus operator.
   *
   * @param lhs Operand of unary minus.
   */
  friend ExpressionPtr operator-(const ExpressionPtr& lhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->is_constant(0.0)) {
      // Return zero
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT) {
      return make_expression_ptr<ConstExpression>(-lhs->val);
    }

    if (lhs->type() == LINEAR) {
      return make_expression_ptr<UnaryMinusExpression<LINEAR>>(lhs);
    } else if (lhs->type() == QUADRATIC) {
      return make_expression_ptr<UnaryMinusExpression<QUADRATIC>>(lhs);
    } else {
      return make_expression_ptr<UnaryMinusExpression<NONLINEAR>>(lhs);
    }
  }

  /**
   * Unary plus operator.
   *
   * @param lhs Operand of unary plus.
   */
  friend ExpressionPtr operator+(const ExpressionPtr& lhs) { return lhs; }

  /**
   * Either nullary operator with no arguments, unary operator with one
   * argument, or binary operator with two arguments. This operator is used to
   * update the node's value.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @return The node's value.
   */
  virtual double value([[maybe_unused]] double lhs,
                       [[maybe_unused]] double rhs) const = 0;

  /**
   * Returns the type of this expression (constant, linear, quadratic, or
   * nonlinear).
   *
   * @return The type of this expression.
   */
  virtual ExpressionType type() const = 0;

  /**
   * Returns double adjoint of the left child expression.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @param parent_adjoint Adjoint of parent expression.
   * @return The double adjoint of the left child expression.
   */
  virtual double grad_l([[maybe_unused]] double lhs,
                        [[maybe_unused]] double rhs,
                        [[maybe_unused]] double parent_adjoint) const {
    return 0.0;
  }

  /**
   * Returns double adjoint of the right child expression.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @param parent_adjoint Adjoint of parent expression.
   * @return The double adjoint of the right child expression.
   */
  virtual double grad_r([[maybe_unused]] double lhs,
                        [[maybe_unused]] double rhs,
                        [[maybe_unused]] double parent_adjoint) const {
    return 0.0;
  }

  /**
   * Returns Expression adjoint of the left child expression.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @param parent_adjoint Adjoint of parent expression.
   * @return The Expression adjoint of the left child expression.
   */
  virtual ExpressionPtr grad_expr_l(
      [[maybe_unused]] const ExpressionPtr& lhs,
      [[maybe_unused]] const ExpressionPtr& rhs,
      [[maybe_unused]] const ExpressionPtr& parent_adjoint) const {
    return make_expression_ptr<ConstExpression>();
  }

  /**
   * Returns Expression adjoint of the right child expression.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @param parent_adjoint Adjoint of parent expression.
   * @return The Expression adjoint of the right child expression.
   */
  virtual ExpressionPtr grad_expr_r(
      [[maybe_unused]] const ExpressionPtr& lhs,
      [[maybe_unused]] const ExpressionPtr& rhs,
      [[maybe_unused]] const ExpressionPtr& parent_adjoint) const {
    return make_expression_ptr<ConstExpression>();
  }
};

inline ExpressionPtr cbrt(const ExpressionPtr& x);
inline ExpressionPtr exp(const ExpressionPtr& x);
inline ExpressionPtr sin(const ExpressionPtr& x);
inline ExpressionPtr sinh(const ExpressionPtr& x);
inline ExpressionPtr sqrt(const ExpressionPtr& x);

/**
 * Derived expression type for binary minus operator.
 *
 * @tparam T Expression type.
 */
template <ExpressionType T>
struct BinaryMinusExpression final : Expression {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr BinaryMinusExpression(ExpressionPtr lhs, ExpressionPtr rhs)
      : Expression{std::move(lhs), std::move(rhs)} {}

  double value(double lhs, double rhs) const override { return lhs - rhs; }

  ExpressionType type() const override { return T; }

  double grad_l(double, double, double parent_adjoint) const override {
    return parent_adjoint;
  }

  double grad_r(double, double, double parent_adjoint) const override {
    return -parent_adjoint;
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr&, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint;
  }

  ExpressionPtr grad_expr_r(
      const ExpressionPtr&, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return -parent_adjoint;
  }
};

/**
 * Derived expression type for binary plus operator.
 *
 * @tparam T Expression type.
 */
template <ExpressionType T>
struct BinaryPlusExpression final : Expression {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr BinaryPlusExpression(ExpressionPtr lhs, ExpressionPtr rhs)
      : Expression{std::move(lhs), std::move(rhs)} {}

  double value(double lhs, double rhs) const override { return lhs + rhs; }

  ExpressionType type() const override { return T; }

  double grad_l(double, double, double parent_adjoint) const override {
    return parent_adjoint;
  }

  double grad_r(double, double, double parent_adjoint) const override {
    return parent_adjoint;
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr&, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint;
  }

  ExpressionPtr grad_expr_r(
      const ExpressionPtr&, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint;
  }
};

/**
 * Derived expression type for std::cbrt().
 */
struct CbrtExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr CbrtExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::cbrt(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    double c = std::cbrt(x);
    return parent_adjoint / (3.0 * c * c);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    auto c = slp::detail::cbrt(x);
    return parent_adjoint / (make_expression_ptr<ConstExpression>(3.0) * c * c);
  }
};

/**
 * std::cbrt() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr cbrt(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Evaluate constant
  if (x->type() == CONSTANT) {
    if (x->val == 0.0) {
      // Return zero
      return x;
    } else if (x->val == -1.0 || x->val == 1.0) {
      return x;
    } else {
      return make_expression_ptr<ConstExpression>(std::cbrt(x->val));
    }
  }

  return make_expression_ptr<CbrtExpression>(x);
}

/**
 * Derived expression type for constant.
 */
struct ConstExpression final : Expression {
  /**
   * Constructs a constant expression with a value of zero.
   */
  constexpr ConstExpression() = default;

  /**
   * Constructs a nullary expression (an operator with no arguments).
   *
   * @param value The expression value.
   */
  explicit constexpr ConstExpression(double value) : Expression{value} {}

  double value(double, double) const override { return val; }

  ExpressionType type() const override { return ExpressionType::CONSTANT; }
};

/**
 * Derived expression type for decision variable.
 */
struct DecisionVariableExpression final : Expression {
  /**
   * Constructs a decision variable expression with a value of zero.
   */
  constexpr DecisionVariableExpression() = default;

  /**
   * Constructs a nullary expression (an operator with no arguments).
   *
   * @param value The expression value.
   */
  explicit constexpr DecisionVariableExpression(double value)
      : Expression{value} {}

  double value(double, double) const override { return val; }

  ExpressionType type() const override { return ExpressionType::LINEAR; }
};

/**
 * Derived expression type for binary division operator.
 *
 * @tparam T Expression type.
 */
template <ExpressionType T>
struct DivExpression final : Expression {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr DivExpression(ExpressionPtr lhs, ExpressionPtr rhs)
      : Expression{std::move(lhs), std::move(rhs)} {}

  double value(double lhs, double rhs) const override { return lhs / rhs; }

  ExpressionType type() const override { return T; }

  double grad_l(double, double rhs, double parent_adjoint) const override {
    return parent_adjoint / rhs;
  };

  double grad_r(double lhs, double rhs, double parent_adjoint) const override {
    return parent_adjoint * -lhs / (rhs * rhs);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr&, const ExpressionPtr& rhs,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint / rhs;
  }

  ExpressionPtr grad_expr_r(
      const ExpressionPtr& lhs, const ExpressionPtr& rhs,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * -lhs / (rhs * rhs);
  }
};

/**
 * Derived expression type for binary multiplication operator.
 *
 * @tparam T Expression type.
 */
template <ExpressionType T>
struct MultExpression final : Expression {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr MultExpression(ExpressionPtr lhs, ExpressionPtr rhs)
      : Expression{std::move(lhs), std::move(rhs)} {}

  double value(double lhs, double rhs) const override { return lhs * rhs; }

  ExpressionType type() const override { return T; }

  double grad_l([[maybe_unused]] double lhs, double rhs,
                double parent_adjoint) const override {
    return parent_adjoint * rhs;
  }

  double grad_r(double lhs, [[maybe_unused]] double rhs,
                double parent_adjoint) const override {
    return parent_adjoint * lhs;
  }

  ExpressionPtr grad_expr_l(
      [[maybe_unused]] const ExpressionPtr& lhs, const ExpressionPtr& rhs,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * rhs;
  }

  ExpressionPtr grad_expr_r(
      const ExpressionPtr& lhs, [[maybe_unused]] const ExpressionPtr& rhs,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * lhs;
  }
};

/**
 * Derived expression type for unary minus operator.
 *
 * @tparam T Expression type.
 */
template <ExpressionType T>
struct UnaryMinusExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr UnaryMinusExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double lhs, double) const override { return -lhs; }

  ExpressionType type() const override { return T; }

  double grad_l(double, double, double parent_adjoint) const override {
    return -parent_adjoint;
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr&, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return -parent_adjoint;
  }
};

/**
 * Refcount increment for intrusive shared pointer.
 *
 * @param expr The shared pointer's managed object.
 */
inline constexpr void inc_ref_count(Expression* expr) {
  ++expr->ref_count;
}

/**
 * Refcount decrement for intrusive shared pointer.
 *
 * @param expr The shared pointer's managed object.
 */
inline void dec_ref_count(Expression* expr) {
  // If a deeply nested tree is being deallocated all at once, calling the
  // Expression destructor when expr's refcount reaches zero can cause a stack
  // overflow. Instead, we iterate over its children to decrement their
  // refcounts and deallocate them.
  gch::small_vector<Expression*> stack;
  stack.emplace_back(expr);

  while (!stack.empty()) {
    auto elem = stack.back();
    stack.pop_back();

    // Decrement the current node's refcount. If it reaches zero, deallocate the
    // node and enqueue its children so their refcounts are decremented too.
    if (--elem->ref_count == 0) {
      if (elem->adjoint_expr != nullptr) {
        stack.emplace_back(elem->adjoint_expr.get());
      }
      for (auto& arg : elem->args) {
        if (arg != nullptr) {
          stack.emplace_back(arg.get());
        }
      }

      // Not calling the destructor here is safe because it only decrements
      // refcounts, which was already done above.
      if constexpr (USE_POOL_ALLOCATOR) {
        auto alloc = global_pool_allocator<Expression>();
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, elem,
                                                           sizeof(Expression));
      }
    }
  }
}

/**
 * Derived expression type for std::abs().
 */
struct AbsExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr AbsExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::abs(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    if (x < 0.0) {
      return -parent_adjoint;
    } else if (x > 0.0) {
      return parent_adjoint;
    } else {
      return 0.0;
    }
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    if (x->val < 0.0) {
      return -parent_adjoint;
    } else if (x->val > 0.0) {
      return parent_adjoint;
    } else {
      // Return zero
      return make_expression_ptr<ConstExpression>();
    }
  }
};

/**
 * std::abs() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr abs(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::abs(x->val));
  }

  return make_expression_ptr<AbsExpression>(x);
}

/**
 * Derived expression type for std::acos().
 */
struct AcosExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr AcosExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::acos(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return -parent_adjoint / std::sqrt(1.0 - x * x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return -parent_adjoint /
           slp::detail::sqrt(make_expression_ptr<ConstExpression>(1.0) - x * x);
  }
};

/**
 * std::acos() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr acos(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    return make_expression_ptr<ConstExpression>(std::numbers::pi / 2.0);
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::acos(x->val));
  }

  return make_expression_ptr<AcosExpression>(x);
}

/**
 * Derived expression type for std::asin().
 */
struct AsinExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr AsinExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::asin(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint / std::sqrt(1.0 - x * x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint /
           slp::detail::sqrt(make_expression_ptr<ConstExpression>(1.0) - x * x);
  }
};

/**
 * std::asin() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr asin(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::asin(x->val));
  }

  return make_expression_ptr<AsinExpression>(x);
}

/**
 * Derived expression type for std::atan().
 */
struct AtanExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr AtanExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::atan(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint / (1.0 + x * x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint / (make_expression_ptr<ConstExpression>(1.0) + x * x);
  }
};

/**
 * std::atan() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr atan(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::atan(x->val));
  }

  return make_expression_ptr<AtanExpression>(x);
}

/**
 * Derived expression type for std::atan2().
 */
struct Atan2Expression final : Expression {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr Atan2Expression(ExpressionPtr lhs, ExpressionPtr rhs)
      : Expression{std::move(lhs), std::move(rhs)} {}

  double value(double y, double x) const override { return std::atan2(y, x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double y, double x, double parent_adjoint) const override {
    return parent_adjoint * x / (y * y + x * x);
  }

  double grad_r(double y, double x, double parent_adjoint) const override {
    return parent_adjoint * -y / (y * y + x * x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& y, const ExpressionPtr& x,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * x / (y * y + x * x);
  }

  ExpressionPtr grad_expr_r(
      const ExpressionPtr& y, const ExpressionPtr& x,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * -y / (y * y + x * x);
  }
};

/**
 * std::atan2() for Expressions.
 *
 * @param y The y argument.
 * @param x The x argument.
 */
inline ExpressionPtr atan2(const ExpressionPtr& y, const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (y->is_constant(0.0)) {
    // Return zero
    return y;
  } else if (x->is_constant(0.0)) {
    return make_expression_ptr<ConstExpression>(std::numbers::pi / 2.0);
  }

  // Evaluate constant
  if (y->type() == CONSTANT && x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::atan2(y->val, x->val));
  }

  return make_expression_ptr<Atan2Expression>(y, x);
}

/**
 * Derived expression type for std::cos().
 */
struct CosExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr CosExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::cos(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return -parent_adjoint * std::sin(x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * -slp::detail::sin(x);
  }
};

/**
 * std::cos() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr cos(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    return make_expression_ptr<ConstExpression>(1.0);
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::cos(x->val));
  }

  return make_expression_ptr<CosExpression>(x);
}

/**
 * Derived expression type for std::cosh().
 */
struct CoshExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr CoshExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::cosh(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint * std::sinh(x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * slp::detail::sinh(x);
  }
};

/**
 * std::cosh() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr cosh(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    return make_expression_ptr<ConstExpression>(1.0);
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::cosh(x->val));
  }

  return make_expression_ptr<CoshExpression>(x);
}

/**
 * Derived expression type for std::erf().
 */
struct ErfExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr ErfExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::erf(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint * 2.0 * std::numbers::inv_sqrtpi * std::exp(-x * x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint *
           make_expression_ptr<ConstExpression>(2.0 *
                                                std::numbers::inv_sqrtpi) *
           slp::detail::exp(-x * x);
  }
};

/**
 * std::erf() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr erf(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::erf(x->val));
  }

  return make_expression_ptr<ErfExpression>(x);
}

/**
 * Derived expression type for std::exp().
 */
struct ExpExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr ExpExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::exp(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint * std::exp(x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * slp::detail::exp(x);
  }
};

/**
 * std::exp() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr exp(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    return make_expression_ptr<ConstExpression>(1.0);
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::exp(x->val));
  }

  return make_expression_ptr<ExpExpression>(x);
}

inline ExpressionPtr hypot(const ExpressionPtr& x, const ExpressionPtr& y);

/**
 * Derived expression type for std::hypot().
 */
struct HypotExpression final : Expression {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr HypotExpression(ExpressionPtr lhs, ExpressionPtr rhs)
      : Expression{std::move(lhs), std::move(rhs)} {}

  double value(double x, double y) const override { return std::hypot(x, y); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double y, double parent_adjoint) const override {
    return parent_adjoint * x / std::hypot(x, y);
  }

  double grad_r(double x, double y, double parent_adjoint) const override {
    return parent_adjoint * y / std::hypot(x, y);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr& y,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * x / slp::detail::hypot(x, y);
  }

  ExpressionPtr grad_expr_r(
      const ExpressionPtr& x, const ExpressionPtr& y,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * y / slp::detail::hypot(x, y);
  }
};

/**
 * std::hypot() for Expressions.
 *
 * @param x The x argument.
 * @param y The y argument.
 */
inline ExpressionPtr hypot(const ExpressionPtr& x, const ExpressionPtr& y) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    return y;
  } else if (y->is_constant(0.0)) {
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT && y->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::hypot(x->val, y->val));
  }

  return make_expression_ptr<HypotExpression>(x, y);
}

/**
 * Derived expression type for std::log().
 */
struct LogExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr LogExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::log(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint / x;
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint / x;
  }
};

/**
 * std::log() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr log(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::log(x->val));
  }

  return make_expression_ptr<LogExpression>(x);
}

/**
 * Derived expression type for std::log10().
 */
struct Log10Expression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr Log10Expression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::log10(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint / (std::numbers::ln10 * x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint /
           (make_expression_ptr<ConstExpression>(std::numbers::ln10) * x);
  }
};

/**
 * std::log10() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr log10(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::log10(x->val));
  }

  return make_expression_ptr<Log10Expression>(x);
}

inline ExpressionPtr pow(const ExpressionPtr& base, const ExpressionPtr& power);

/**
 * Derived expression type for std::pow().
 *
 * @tparam Expression type.
 */
template <ExpressionType T>
struct PowExpression final : Expression {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr PowExpression(ExpressionPtr lhs, ExpressionPtr rhs)
      : Expression{std::move(lhs), std::move(rhs)} {}

  double value(double base, double power) const override {
    return std::pow(base, power);
  }

  ExpressionType type() const override { return T; }

  double grad_l(double base, double power,
                double parent_adjoint) const override {
    return parent_adjoint * std::pow(base, power - 1) * power;
  }

  double grad_r(double base, double power,
                double parent_adjoint) const override {
    // Since x * std::log(x) -> 0 as x -> 0
    if (base == 0.0) {
      return 0.0;
    } else {
      return parent_adjoint * std::pow(base, power - 1) * base * std::log(base);
    }
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& base, const ExpressionPtr& power,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint *
           slp::detail::pow(base,
                            power - make_expression_ptr<ConstExpression>(1.0)) *
           power;
  }

  ExpressionPtr grad_expr_r(
      const ExpressionPtr& base, const ExpressionPtr& power,
      const ExpressionPtr& parent_adjoint) const override {
    // Since x * std::log(x) -> 0 as x -> 0
    if (base->val == 0.0) {
      // Return zero
      return base;
    } else {
      return parent_adjoint *
             slp::detail::pow(
                 base, power - make_expression_ptr<ConstExpression>(1.0)) *
             base * slp::detail::log(base);
    }
  }
};

/**
 * std::pow() for Expressions.
 *
 * @param base The base.
 * @param power The power.
 */
inline ExpressionPtr pow(const ExpressionPtr& base,
                         const ExpressionPtr& power) {
  using enum ExpressionType;

  // Prune expression
  if (base->is_constant(0.0)) {
    // Return zero
    return base;
  } else if (base->is_constant(1.0)) {
    // Return one
    return base;
  }
  if (power->is_constant(0.0)) {
    return make_expression_ptr<ConstExpression>(1.0);
  } else if (power->is_constant(1.0)) {
    return base;
  }

  // Evaluate constant
  if (base->type() == CONSTANT && power->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(
        std::pow(base->val, power->val));
  }

  if (power->is_constant(2.0)) {
    if (base->type() == LINEAR) {
      return make_expression_ptr<MultExpression<QUADRATIC>>(base, base);
    } else {
      return make_expression_ptr<MultExpression<NONLINEAR>>(base, base);
    }
  }

  return make_expression_ptr<PowExpression<NONLINEAR>>(base, power);
}

/**
 * Derived expression type for sign().
 */
struct SignExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr SignExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override {
    if (x < 0.0) {
      return -1.0;
    } else if (x == 0.0) {
      return 0.0;
    } else {
      return 1.0;
    }
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double, double, double) const override { return 0.0; }

  ExpressionPtr grad_expr_l(const ExpressionPtr&, const ExpressionPtr&,
                            const ExpressionPtr&) const override {
    // Return zero
    return make_expression_ptr<ConstExpression>();
  }
};

/**
 * sign() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr sign(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Evaluate constant
  if (x->type() == CONSTANT) {
    if (x->val < 0.0) {
      return make_expression_ptr<ConstExpression>(-1.0);
    } else if (x->val == 0.0) {
      // Return zero
      return x;
    } else {
      return make_expression_ptr<ConstExpression>(1.0);
    }
  }

  return make_expression_ptr<SignExpression>(x);
}

/**
 * Derived expression type for std::sin().
 */
struct SinExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr SinExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::sin(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint * std::cos(x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * slp::detail::cos(x);
  }
};

/**
 * std::sin() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr sin(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::sin(x->val));
  }

  return make_expression_ptr<SinExpression>(x);
}

/**
 * Derived expression type for std::sinh().
 */
struct SinhExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr SinhExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::sinh(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint * std::cosh(x);
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint * slp::detail::cosh(x);
  }
};

/**
 * std::sinh() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr sinh(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::sinh(x->val));
  }

  return make_expression_ptr<SinhExpression>(x);
}

/**
 * Derived expression type for std::sqrt().
 */
struct SqrtExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr SqrtExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::sqrt(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint / (2.0 * std::sqrt(x));
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint /
           (make_expression_ptr<ConstExpression>(2.0) * slp::detail::sqrt(x));
  }
};

/**
 * std::sqrt() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr sqrt(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Evaluate constant
  if (x->type() == CONSTANT) {
    if (x->val == 0.0) {
      // Return zero
      return x;
    } else if (x->val == 1.0) {
      return x;
    } else {
      return make_expression_ptr<ConstExpression>(std::sqrt(x->val));
    }
  }

  return make_expression_ptr<SqrtExpression>(x);
}

/**
 * Derived expression type for std::tan().
 */
struct TanExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr TanExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::tan(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint / (std::cos(x) * std::cos(x));
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint / (slp::detail::cos(x) * slp::detail::cos(x));
  }
};

/**
 * std::tan() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr tan(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::tan(x->val));
  }

  return make_expression_ptr<TanExpression>(x);
}

/**
 * Derived expression type for std::tanh().
 */
struct TanhExpression final : Expression {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr TanhExpression(ExpressionPtr lhs)
      : Expression{std::move(lhs)} {}

  double value(double x, double) const override { return std::tanh(x); }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  double grad_l(double x, double, double parent_adjoint) const override {
    return parent_adjoint / (std::cosh(x) * std::cosh(x));
  }

  ExpressionPtr grad_expr_l(
      const ExpressionPtr& x, const ExpressionPtr&,
      const ExpressionPtr& parent_adjoint) const override {
    return parent_adjoint / (slp::detail::cosh(x) * slp::detail::cosh(x));
  }
};

/**
 * std::tanh() for Expressions.
 *
 * @param x The argument.
 */
inline ExpressionPtr tanh(const ExpressionPtr& x) {
  using enum ExpressionType;

  // Prune expression
  if (x->is_constant(0.0)) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return make_expression_ptr<ConstExpression>(std::tanh(x->val));
  }

  return make_expression_ptr<TanhExpression>(x);
}

}  // namespace slp::detail
