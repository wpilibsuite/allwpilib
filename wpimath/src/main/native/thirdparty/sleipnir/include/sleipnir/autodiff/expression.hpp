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

template <typename Scalar>
struct Expression;

template <typename Scalar>
constexpr void inc_ref_count(Expression<Scalar>* expr);
template <typename Scalar>
void dec_ref_count(Expression<Scalar>* expr);

/**
 * Typedef for intrusive shared pointer to Expression.
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
using ExpressionPtr = IntrusiveSharedPtr<Expression<Scalar>>;

/**
 * Creates an intrusive shared pointer to an expression from the global pool
 * allocator.
 *
 * @tparam T The derived expression type.
 * @param args Constructor arguments for Expression.
 */
template <typename T, typename... Args>
static ExpressionPtr<typename T::Scalar> make_expression_ptr(Args&&... args) {
  if constexpr (USE_POOL_ALLOCATOR) {
    return allocate_intrusive_shared<T>(global_pool_allocator<T>(),
                                        std::forward<Args>(args)...);
  } else {
    return make_intrusive_shared<T>(std::forward<Args>(args)...);
  }
}

template <typename Scalar, ExpressionType T>
struct BinaryMinusExpression;

template <typename Scalar, ExpressionType T>
struct BinaryPlusExpression;

template <typename Scalar>
struct ConstExpression;

template <typename Scalar, ExpressionType T>
struct DivExpression;

template <typename Scalar, ExpressionType T>
struct MultExpression;

template <typename Scalar, ExpressionType T>
struct UnaryMinusExpression;

/**
 * Creates an intrusive shared pointer to a constant expression.
 *
 * @tparam Scalar Scalar type.
 * @param value The expression value.
 */
template <typename Scalar>
ExpressionPtr<Scalar> constant_ptr(Scalar value);

/**
 * An autodiff expression node.
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar_>
struct Expression {
  /**
   * Scalar type alias.
   */
  using Scalar = Scalar_;

  /// The value of the expression node.
  Scalar val{0};

  /// The adjoint of the expression node, used during autodiff.
  Scalar adjoint{0};

  /// Counts incoming edges for this node.
  uint32_t incoming_edges = 0;

  /// This expression's column in a Jacobian, or -1 otherwise.
  int32_t col = -1;

  /// The adjoint of the expression node, used during gradient expression tree
  /// generation.
  ExpressionPtr<Scalar> adjoint_expr;

  /// Reference count for intrusive shared pointer.
  uint32_t ref_count = 0;

  /// Expression arguments.
  std::array<ExpressionPtr<Scalar>, 2> args{nullptr, nullptr};

  /**
   * Constructs a constant expression with a value of zero.
   */
  constexpr Expression() = default;

  /**
   * Constructs a nullary expression (an operator with no arguments).
   *
   * @param value The expression value.
   */
  explicit constexpr Expression(Scalar value) : val{value} {}

  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr Expression(ExpressionPtr<Scalar> lhs)
      : args{std::move(lhs), nullptr} {}

  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr Expression(ExpressionPtr<Scalar> lhs, ExpressionPtr<Scalar> rhs)
      : args{std::move(lhs), std::move(rhs)} {}

  virtual ~Expression() = default;

  /**
   * Returns true if the expression is the given constant.
   *
   * @param constant The constant.
   *
   * @return True if the expression is the given constant.
   */
  constexpr bool is_constant(Scalar constant) const {
    return type() == ExpressionType::CONSTANT && val == constant;
  }

  /**
   * Expression-Expression multiplication operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr<Scalar> operator*(const ExpressionPtr<Scalar>& lhs,
                                         const ExpressionPtr<Scalar>& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->is_constant(Scalar(0))) {
      // Return zero
      return lhs;
    } else if (rhs->is_constant(Scalar(0))) {
      // Return zero
      return rhs;
    } else if (lhs->is_constant(Scalar(1))) {
      return rhs;
    } else if (rhs->is_constant(Scalar(1))) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT && rhs->type() == CONSTANT) {
      return constant_ptr(lhs->val * rhs->val);
    }

    // Evaluate expression type
    if (lhs->type() == CONSTANT) {
      if (rhs->type() == LINEAR) {
        return make_expression_ptr<MultExpression<Scalar, LINEAR>>(lhs, rhs);
      } else if (rhs->type() == QUADRATIC) {
        return make_expression_ptr<MultExpression<Scalar, QUADRATIC>>(lhs, rhs);
      } else {
        return make_expression_ptr<MultExpression<Scalar, NONLINEAR>>(lhs, rhs);
      }
    } else if (rhs->type() == CONSTANT) {
      if (lhs->type() == LINEAR) {
        return make_expression_ptr<MultExpression<Scalar, LINEAR>>(lhs, rhs);
      } else if (lhs->type() == QUADRATIC) {
        return make_expression_ptr<MultExpression<Scalar, QUADRATIC>>(lhs, rhs);
      } else {
        return make_expression_ptr<MultExpression<Scalar, NONLINEAR>>(lhs, rhs);
      }
    } else if (lhs->type() == LINEAR && rhs->type() == LINEAR) {
      return make_expression_ptr<MultExpression<Scalar, QUADRATIC>>(lhs, rhs);
    } else {
      return make_expression_ptr<MultExpression<Scalar, NONLINEAR>>(lhs, rhs);
    }
  }

  /**
   * Expression-Expression division operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr<Scalar> operator/(const ExpressionPtr<Scalar>& lhs,
                                         const ExpressionPtr<Scalar>& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->is_constant(Scalar(0))) {
      // Return zero
      return lhs;
    } else if (rhs->is_constant(Scalar(1))) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT && rhs->type() == CONSTANT) {
      return constant_ptr(lhs->val / rhs->val);
    }

    // Evaluate expression type
    if (rhs->type() == CONSTANT) {
      if (lhs->type() == LINEAR) {
        return make_expression_ptr<DivExpression<Scalar, LINEAR>>(lhs, rhs);
      } else if (lhs->type() == QUADRATIC) {
        return make_expression_ptr<DivExpression<Scalar, QUADRATIC>>(lhs, rhs);
      } else {
        return make_expression_ptr<DivExpression<Scalar, NONLINEAR>>(lhs, rhs);
      }
    } else {
      return make_expression_ptr<DivExpression<Scalar, NONLINEAR>>(lhs, rhs);
    }
  }

  /**
   * Expression-Expression addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr<Scalar> operator+(const ExpressionPtr<Scalar>& lhs,
                                         const ExpressionPtr<Scalar>& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs == nullptr || lhs->is_constant(Scalar(0))) {
      return rhs;
    } else if (rhs == nullptr || rhs->is_constant(Scalar(0))) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT && rhs->type() == CONSTANT) {
      return constant_ptr(lhs->val + rhs->val);
    }

    auto type = std::max(lhs->type(), rhs->type());
    if (type == LINEAR) {
      return make_expression_ptr<BinaryPlusExpression<Scalar, LINEAR>>(lhs,
                                                                       rhs);
    } else if (type == QUADRATIC) {
      return make_expression_ptr<BinaryPlusExpression<Scalar, QUADRATIC>>(lhs,
                                                                          rhs);
    } else {
      return make_expression_ptr<BinaryPlusExpression<Scalar, NONLINEAR>>(lhs,
                                                                          rhs);
    }
  }

  /**
   * Expression-Expression compound addition operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr<Scalar> operator+=(ExpressionPtr<Scalar>& lhs,
                                          const ExpressionPtr<Scalar>& rhs) {
    return lhs = lhs + rhs;
  }

  /**
   * Expression-Expression subtraction operator.
   *
   * @param lhs Operator left-hand side.
   * @param rhs Operator right-hand side.
   */
  friend ExpressionPtr<Scalar> operator-(const ExpressionPtr<Scalar>& lhs,
                                         const ExpressionPtr<Scalar>& rhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->is_constant(Scalar(0))) {
      if (rhs->is_constant(Scalar(0))) {
        // Return zero
        return rhs;
      } else {
        return -rhs;
      }
    } else if (rhs->is_constant(Scalar(0))) {
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT && rhs->type() == CONSTANT) {
      return constant_ptr(lhs->val - rhs->val);
    }

    auto type = std::max(lhs->type(), rhs->type());
    if (type == LINEAR) {
      return make_expression_ptr<BinaryMinusExpression<Scalar, LINEAR>>(lhs,
                                                                        rhs);
    } else if (type == QUADRATIC) {
      return make_expression_ptr<BinaryMinusExpression<Scalar, QUADRATIC>>(lhs,
                                                                           rhs);
    } else {
      return make_expression_ptr<BinaryMinusExpression<Scalar, NONLINEAR>>(lhs,
                                                                           rhs);
    }
  }

  /**
   * Unary minus operator.
   *
   * @param lhs Operand of unary minus.
   */
  friend ExpressionPtr<Scalar> operator-(const ExpressionPtr<Scalar>& lhs) {
    using enum ExpressionType;

    // Prune expression
    if (lhs->is_constant(Scalar(0))) {
      // Return zero
      return lhs;
    }

    // Evaluate constant
    if (lhs->type() == CONSTANT) {
      return constant_ptr(-lhs->val);
    }

    if (lhs->type() == LINEAR) {
      return make_expression_ptr<UnaryMinusExpression<Scalar, LINEAR>>(lhs);
    } else if (lhs->type() == QUADRATIC) {
      return make_expression_ptr<UnaryMinusExpression<Scalar, QUADRATIC>>(lhs);
    } else {
      return make_expression_ptr<UnaryMinusExpression<Scalar, NONLINEAR>>(lhs);
    }
  }

  /**
   * Unary plus operator.
   *
   * @param lhs Operand of unary plus.
   */
  friend ExpressionPtr<Scalar> operator+(const ExpressionPtr<Scalar>& lhs) {
    return lhs;
  }

  /**
   * Either nullary operator with no arguments, unary operator with one
   * argument, or binary operator with two arguments. This operator is used to
   * update the node's value.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @return The node's value.
   */
  virtual Scalar value([[maybe_unused]] Scalar lhs,
                       [[maybe_unused]] Scalar rhs) const = 0;

  /**
   * Returns the type of this expression (constant, linear, quadratic, or
   * nonlinear).
   *
   * @return The type of this expression.
   */
  virtual ExpressionType type() const = 0;

  /**
   * Returns ∂/∂l as a Scalar.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @param parent_adjoint Adjoint of parent expression.
   * @return ∂/∂l as a Scalar.
   */
  virtual Scalar grad_l([[maybe_unused]] Scalar lhs,
                        [[maybe_unused]] Scalar rhs,
                        [[maybe_unused]] Scalar parent_adjoint) const {
    return Scalar(0);
  }

  /**
   * Returns ∂/∂r as a Scalar.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @param parent_adjoint Adjoint of parent expression.
   * @return ∂/∂r as a Scalar.
   */
  virtual Scalar grad_r([[maybe_unused]] Scalar lhs,
                        [[maybe_unused]] Scalar rhs,
                        [[maybe_unused]] Scalar parent_adjoint) const {
    return Scalar(0);
  }

  /**
   * Returns ∂/∂l as an Expression.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @param parent_adjoint Adjoint of parent expression.
   * @return ∂/∂l as an Expression.
   */
  virtual ExpressionPtr<Scalar> grad_expr_l(
      [[maybe_unused]] const ExpressionPtr<Scalar>& lhs,
      [[maybe_unused]] const ExpressionPtr<Scalar>& rhs,
      [[maybe_unused]] const ExpressionPtr<Scalar>& parent_adjoint) const {
    return constant_ptr(Scalar(0));
  }

  /**
   * Returns ∂/∂r as an Expression.
   *
   * @param lhs Left argument to binary operator.
   * @param rhs Right argument to binary operator.
   * @param parent_adjoint Adjoint of parent expression.
   * @return ∂/∂r as an Expression.
   */
  virtual ExpressionPtr<Scalar> grad_expr_r(
      [[maybe_unused]] const ExpressionPtr<Scalar>& lhs,
      [[maybe_unused]] const ExpressionPtr<Scalar>& rhs,
      [[maybe_unused]] const ExpressionPtr<Scalar>& parent_adjoint) const {
    return constant_ptr(Scalar(0));
  }
};

template <typename Scalar>
ExpressionPtr<Scalar> constant_ptr(Scalar value) {
  return make_expression_ptr<ConstExpression<Scalar>>(value);
}

template <typename Scalar>
ExpressionPtr<Scalar> cbrt(const ExpressionPtr<Scalar>& x);
template <typename Scalar>
ExpressionPtr<Scalar> exp(const ExpressionPtr<Scalar>& x);
template <typename Scalar>
ExpressionPtr<Scalar> sin(const ExpressionPtr<Scalar>& x);
template <typename Scalar>
ExpressionPtr<Scalar> sinh(const ExpressionPtr<Scalar>& x);
template <typename Scalar>
ExpressionPtr<Scalar> sqrt(const ExpressionPtr<Scalar>& x);

/**
 * Derived expression type for binary minus operator.
 *
 * @tparam Scalar Scalar type.
 * @tparam T Expression type.
 */
template <typename Scalar, ExpressionType T>
struct BinaryMinusExpression final : Expression<Scalar> {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr BinaryMinusExpression(ExpressionPtr<Scalar> lhs,
                                  ExpressionPtr<Scalar> rhs)
      : Expression<Scalar>{std::move(lhs), std::move(rhs)} {}

  Scalar value(Scalar lhs, Scalar rhs) const override { return lhs - rhs; }

  ExpressionType type() const override { return T; }

  Scalar grad_l(Scalar, Scalar, Scalar parent_adjoint) const override {
    return parent_adjoint;
  }

  Scalar grad_r(Scalar, Scalar, Scalar parent_adjoint) const override {
    return -parent_adjoint;
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>&, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint;
  }

  ExpressionPtr<Scalar> grad_expr_r(
      const ExpressionPtr<Scalar>&, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return -parent_adjoint;
  }
};

/**
 * Derived expression type for binary plus operator.
 *
 * @tparam Scalar Scalar type.
 * @tparam T Expression type.
 */
template <typename Scalar, ExpressionType T>
struct BinaryPlusExpression final : Expression<Scalar> {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr BinaryPlusExpression(ExpressionPtr<Scalar> lhs,
                                 ExpressionPtr<Scalar> rhs)
      : Expression<Scalar>{std::move(lhs), std::move(rhs)} {}

  Scalar value(Scalar lhs, Scalar rhs) const override { return lhs + rhs; }

  ExpressionType type() const override { return T; }

  Scalar grad_l(Scalar, Scalar, Scalar parent_adjoint) const override {
    return parent_adjoint;
  }

  Scalar grad_r(Scalar, Scalar, Scalar parent_adjoint) const override {
    return parent_adjoint;
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>&, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint;
  }

  ExpressionPtr<Scalar> grad_expr_r(
      const ExpressionPtr<Scalar>&, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint;
  }
};

/**
 * Derived expression type for cbrt().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct CbrtExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr CbrtExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::cbrt;
    return cbrt(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::cbrt;

    Scalar c = cbrt(x);
    return parent_adjoint / (Scalar(3) * c * c);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    auto c = cbrt(x);
    return parent_adjoint / (constant_ptr(Scalar(3)) * c * c);
  }
};

/**
 * cbrt() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> cbrt(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::cbrt;

  // Evaluate constant
  if (x->type() == CONSTANT) {
    if (x->val == Scalar(0)) {
      // Return zero
      return x;
    } else if (x->val == Scalar(-1) || x->val == Scalar(1)) {
      return x;
    } else {
      return constant_ptr(cbrt(x->val));
    }
  }

  return make_expression_ptr<CbrtExpression<Scalar>>(x);
}

/**
 * Derived expression type for constant.
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct ConstExpression final : Expression<Scalar> {
  /**
   * Constructs a nullary expression (an operator with no arguments).
   *
   * @param value The expression value.
   */
  explicit constexpr ConstExpression(Scalar value)
      : Expression<Scalar>{value} {}

  Scalar value(Scalar, Scalar) const override { return this->val; }

  ExpressionType type() const override { return ExpressionType::CONSTANT; }
};

/**
 * Derived expression type for decision variable.
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct DecisionVariableExpression final : Expression<Scalar> {
  /**
   * Constructs a decision variable expression with a value of zero.
   */
  constexpr DecisionVariableExpression() = default;

  /**
   * Constructs a nullary expression (an operator with no arguments).
   *
   * @param value The expression value.
   */
  explicit constexpr DecisionVariableExpression(Scalar value)
      : Expression<Scalar>{value} {}

  Scalar value(Scalar, Scalar) const override { return this->val; }

  ExpressionType type() const override { return ExpressionType::LINEAR; }
};

/**
 * Derived expression type for binary division operator.
 *
 * @tparam Scalar Scalar type.
 * @tparam T Expression type.
 */
template <typename Scalar, ExpressionType T>
struct DivExpression final : Expression<Scalar> {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr DivExpression(ExpressionPtr<Scalar> lhs, ExpressionPtr<Scalar> rhs)
      : Expression<Scalar>{std::move(lhs), std::move(rhs)} {}

  Scalar value(Scalar lhs, Scalar rhs) const override { return lhs / rhs; }

  ExpressionType type() const override { return T; }

  Scalar grad_l(Scalar, Scalar rhs, Scalar parent_adjoint) const override {
    return parent_adjoint / rhs;
  };

  Scalar grad_r(Scalar lhs, Scalar rhs, Scalar parent_adjoint) const override {
    return parent_adjoint * -lhs / (rhs * rhs);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>&, const ExpressionPtr<Scalar>& rhs,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint / rhs;
  }

  ExpressionPtr<Scalar> grad_expr_r(
      const ExpressionPtr<Scalar>& lhs, const ExpressionPtr<Scalar>& rhs,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * -lhs / (rhs * rhs);
  }
};

/**
 * Derived expression type for binary multiplication operator.
 *
 * @tparam Scalar Scalar type.
 * @tparam T Expression type.
 */
template <typename Scalar, ExpressionType T>
struct MultExpression final : Expression<Scalar> {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr MultExpression(ExpressionPtr<Scalar> lhs, ExpressionPtr<Scalar> rhs)
      : Expression<Scalar>{std::move(lhs), std::move(rhs)} {}

  Scalar value(Scalar lhs, Scalar rhs) const override { return lhs * rhs; }

  ExpressionType type() const override { return T; }

  Scalar grad_l([[maybe_unused]] Scalar lhs, Scalar rhs,
                Scalar parent_adjoint) const override {
    return parent_adjoint * rhs;
  }

  Scalar grad_r(Scalar lhs, [[maybe_unused]] Scalar rhs,
                Scalar parent_adjoint) const override {
    return parent_adjoint * lhs;
  }

  ExpressionPtr<Scalar> grad_expr_l(
      [[maybe_unused]] const ExpressionPtr<Scalar>& lhs,
      const ExpressionPtr<Scalar>& rhs,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * rhs;
  }

  ExpressionPtr<Scalar> grad_expr_r(
      const ExpressionPtr<Scalar>& lhs,
      [[maybe_unused]] const ExpressionPtr<Scalar>& rhs,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * lhs;
  }
};

/**
 * Derived expression type for unary minus operator.
 *
 * @tparam Scalar Scalar type.
 * @tparam T Expression type.
 */
template <typename Scalar, ExpressionType T>
struct UnaryMinusExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr UnaryMinusExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar lhs, Scalar) const override { return -lhs; }

  ExpressionType type() const override { return T; }

  Scalar grad_l(Scalar, Scalar, Scalar parent_adjoint) const override {
    return -parent_adjoint;
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>&, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return -parent_adjoint;
  }
};

/**
 * Refcount increment for intrusive shared pointer.
 *
 * @tparam Scalar Scalar type.
 * @param expr The shared pointer's managed object.
 */
template <typename Scalar>
constexpr void inc_ref_count(Expression<Scalar>* expr) {
  ++expr->ref_count;
}

/**
 * Refcount decrement for intrusive shared pointer.
 *
 * @tparam Scalar Scalar type.
 * @param expr The shared pointer's managed object.
 */
template <typename Scalar>
void dec_ref_count(Expression<Scalar>* expr) {
  // If a deeply nested tree is being deallocated all at once, calling the
  // Expression destructor when expr's refcount reaches zero can cause a stack
  // overflow. Instead, we iterate over its children to decrement their
  // refcounts and deallocate them.
  gch::small_vector<Expression<Scalar>*> stack;
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
        auto alloc = global_pool_allocator<Expression<Scalar>>();
        std::allocator_traits<decltype(alloc)>::deallocate(
            alloc, elem, sizeof(Expression<Scalar>));
      }
    }
  }
}

/**
 * Derived expression type for abs().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct AbsExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr AbsExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::abs;
    return abs(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    if (x < Scalar(0)) {
      return -parent_adjoint;
    } else if (x > Scalar(0)) {
      return parent_adjoint;
    } else {
      return Scalar(0);
    }
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    if (x->val < Scalar(0)) {
      return -parent_adjoint;
    } else if (x->val > Scalar(0)) {
      return parent_adjoint;
    } else {
      return constant_ptr(Scalar(0));
    }
  }
};

/**
 * abs() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> abs(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::abs;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(abs(x->val));
  }

  return make_expression_ptr<AbsExpression<Scalar>>(x);
}

/**
 * Derived expression type for acos().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct AcosExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr AcosExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::acos;
    return acos(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::sqrt;
    return -parent_adjoint / sqrt(Scalar(1) - x * x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return -parent_adjoint / sqrt(constant_ptr(Scalar(1)) - x * x);
  }
};

/**
 * acos() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> acos(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::acos;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    return constant_ptr(Scalar(std::numbers::pi) / Scalar(2));
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(acos(x->val));
  }

  return make_expression_ptr<AcosExpression<Scalar>>(x);
}

/**
 * Derived expression type for asin().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct AsinExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr AsinExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::asin;
    return asin(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::sqrt;
    return parent_adjoint / sqrt(Scalar(1) - x * x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint / sqrt(constant_ptr(Scalar(1)) - x * x);
  }
};

/**
 * asin() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> asin(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::asin;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(asin(x->val));
  }

  return make_expression_ptr<AsinExpression<Scalar>>(x);
}

/**
 * Derived expression type for atan().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct AtanExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr AtanExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::atan;
    return atan(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    return parent_adjoint / (Scalar(1) + x * x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint / (constant_ptr(Scalar(1)) + x * x);
  }
};

/**
 * atan() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> atan(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::atan;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(atan(x->val));
  }

  return make_expression_ptr<AtanExpression<Scalar>>(x);
}

/**
 * Derived expression type for atan2().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct Atan2Expression final : Expression<Scalar> {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr Atan2Expression(ExpressionPtr<Scalar> lhs,
                            ExpressionPtr<Scalar> rhs)
      : Expression<Scalar>{std::move(lhs), std::move(rhs)} {}

  Scalar value(Scalar y, Scalar x) const override {
    using std::atan2;
    return atan2(y, x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar y, Scalar x, Scalar parent_adjoint) const override {
    return parent_adjoint * x / (y * y + x * x);
  }

  Scalar grad_r(Scalar y, Scalar x, Scalar parent_adjoint) const override {
    return parent_adjoint * -y / (y * y + x * x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& y, const ExpressionPtr<Scalar>& x,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * x / (y * y + x * x);
  }

  ExpressionPtr<Scalar> grad_expr_r(
      const ExpressionPtr<Scalar>& y, const ExpressionPtr<Scalar>& x,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * -y / (y * y + x * x);
  }
};

/**
 * atan2() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param y The y argument.
 * @param x The x argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> atan2(const ExpressionPtr<Scalar>& y,
                            const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::atan2;

  // Prune expression
  if (y->is_constant(Scalar(0))) {
    // Return zero
    return y;
  } else if (x->is_constant(Scalar(0))) {
    return constant_ptr(Scalar(std::numbers::pi) / Scalar(2));
  }

  // Evaluate constant
  if (y->type() == CONSTANT && x->type() == CONSTANT) {
    return constant_ptr(atan2(y->val, x->val));
  }

  return make_expression_ptr<Atan2Expression<Scalar>>(y, x);
}

/**
 * Derived expression type for cos().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct CosExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr CosExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::cos;
    return cos(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::sin;
    return parent_adjoint * -sin(x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * -sin(x);
  }
};

/**
 * cos() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> cos(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::cos;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    return constant_ptr(Scalar(1));
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(cos(x->val));
  }

  return make_expression_ptr<CosExpression<Scalar>>(x);
}

/**
 * Derived expression type for cosh().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct CoshExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr CoshExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::cosh;
    return cosh(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::sinh;
    return parent_adjoint * sinh(x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * sinh(x);
  }
};

/**
 * cosh() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> cosh(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::cosh;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    return constant_ptr(Scalar(1));
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(cosh(x->val));
  }

  return make_expression_ptr<CoshExpression<Scalar>>(x);
}

/**
 * Derived expression type for erf().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct ErfExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr ErfExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::erf;
    return erf(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::exp;
    return parent_adjoint * Scalar(2.0 * std::numbers::inv_sqrtpi) *
           exp(-x * x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint *
           constant_ptr(Scalar(2.0 * std::numbers::inv_sqrtpi)) * exp(-x * x);
  }
};

/**
 * erf() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> erf(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::erf;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(erf(x->val));
  }

  return make_expression_ptr<ErfExpression<Scalar>>(x);
}

/**
 * Derived expression type for exp().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct ExpExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr ExpExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::exp;
    return exp(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::exp;
    return parent_adjoint * exp(x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * exp(x);
  }
};

/**
 * exp() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> exp(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::exp;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    return constant_ptr(Scalar(1));
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(exp(x->val));
  }

  return make_expression_ptr<ExpExpression<Scalar>>(x);
}

template <typename Scalar>
ExpressionPtr<Scalar> hypot(const ExpressionPtr<Scalar>& x,
                            const ExpressionPtr<Scalar>& y);

/**
 * Derived expression type for hypot().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct HypotExpression final : Expression<Scalar> {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr HypotExpression(ExpressionPtr<Scalar> lhs,
                            ExpressionPtr<Scalar> rhs)
      : Expression<Scalar>{std::move(lhs), std::move(rhs)} {}

  Scalar value(Scalar x, Scalar y) const override {
    using std::hypot;
    return hypot(x, y);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar y, Scalar parent_adjoint) const override {
    using std::hypot;
    return parent_adjoint * x / hypot(x, y);
  }

  Scalar grad_r(Scalar x, Scalar y, Scalar parent_adjoint) const override {
    using std::hypot;
    return parent_adjoint * y / hypot(x, y);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>& y,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * x / hypot(x, y);
  }

  ExpressionPtr<Scalar> grad_expr_r(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>& y,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * y / hypot(x, y);
  }
};

/**
 * hypot() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The x argument.
 * @param y The y argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> hypot(const ExpressionPtr<Scalar>& x,
                            const ExpressionPtr<Scalar>& y) {
  using enum ExpressionType;
  using std::hypot;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    return y;
  } else if (y->is_constant(Scalar(0))) {
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT && y->type() == CONSTANT) {
    return constant_ptr(hypot(x->val, y->val));
  }

  return make_expression_ptr<HypotExpression<Scalar>>(x, y);
}

/**
 * Derived expression type for log().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct LogExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr LogExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::log;
    return log(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    return parent_adjoint / x;
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint / x;
  }
};

/**
 * log() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> log(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::log;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(log(x->val));
  }

  return make_expression_ptr<LogExpression<Scalar>>(x);
}

/**
 * Derived expression type for log10().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct Log10Expression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr Log10Expression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::log10;
    return log10(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    return parent_adjoint / (Scalar(std::numbers::ln10) * x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint / (constant_ptr(Scalar(std::numbers::ln10)) * x);
  }
};

/**
 * log10() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> log10(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::log10;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(log10(x->val));
  }

  return make_expression_ptr<Log10Expression<Scalar>>(x);
}

template <typename Scalar>
ExpressionPtr<Scalar> pow(const ExpressionPtr<Scalar>& base,
                          const ExpressionPtr<Scalar>& power);

/**
 * Derived expression type for pow().
 *
 * @tparam Scalar Scalar type.
 * @tparam T Expression type.
 */
template <typename Scalar, ExpressionType T>
struct PowExpression final : Expression<Scalar> {
  /**
   * Constructs a binary expression (an operator with two arguments).
   *
   * @param lhs Binary operator's left operand.
   * @param rhs Binary operator's right operand.
   */
  constexpr PowExpression(ExpressionPtr<Scalar> lhs, ExpressionPtr<Scalar> rhs)
      : Expression<Scalar>{std::move(lhs), std::move(rhs)} {}

  Scalar value(Scalar base, Scalar power) const override {
    using std::pow;
    return pow(base, power);
  }

  ExpressionType type() const override { return T; }

  Scalar grad_l(Scalar base, Scalar power,
                Scalar parent_adjoint) const override {
    using std::pow;
    return parent_adjoint * pow(base, power - Scalar(1)) * power;
  }

  Scalar grad_r(Scalar base, Scalar power,
                Scalar parent_adjoint) const override {
    using std::log;
    using std::pow;

    // Since x log(x) -> 0 as x -> 0
    if (base == Scalar(0)) {
      return Scalar(0);
    } else {
      return parent_adjoint * pow(base, power) * log(base);
    }
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& base, const ExpressionPtr<Scalar>& power,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * pow(base, power - constant_ptr(Scalar(1))) * power;
  }

  ExpressionPtr<Scalar> grad_expr_r(
      const ExpressionPtr<Scalar>& base, const ExpressionPtr<Scalar>& power,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    // Since x log(x) -> 0 as x -> 0
    if (base->val == Scalar(0)) {
      // Return zero
      return base;
    } else {
      return parent_adjoint * pow(base, power) * log(base);
    }
  }
};

/**
 * pow() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param base The base.
 * @param power The power.
 */
template <typename Scalar>
ExpressionPtr<Scalar> pow(const ExpressionPtr<Scalar>& base,
                          const ExpressionPtr<Scalar>& power) {
  using enum ExpressionType;
  using std::pow;

  // Prune expression
  if (base->is_constant(Scalar(0))) {
    // Return zero
    return base;
  } else if (base->is_constant(Scalar(1))) {
    // Return one
    return base;
  }
  if (power->is_constant(Scalar(0))) {
    return constant_ptr(Scalar(1));
  } else if (power->is_constant(Scalar(1))) {
    return base;
  }

  // Evaluate constant
  if (base->type() == CONSTANT && power->type() == CONSTANT) {
    return constant_ptr(pow(base->val, power->val));
  }

  if (power->is_constant(Scalar(2))) {
    if (base->type() == LINEAR) {
      return make_expression_ptr<MultExpression<Scalar, QUADRATIC>>(base, base);
    } else {
      return make_expression_ptr<MultExpression<Scalar, NONLINEAR>>(base, base);
    }
  }

  return make_expression_ptr<PowExpression<Scalar, NONLINEAR>>(base, power);
}

/**
 * Derived expression type for sign().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct SignExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr SignExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    if (x < Scalar(0)) {
      return Scalar(-1);
    } else if (x == Scalar(0)) {
      return Scalar(0);
    } else {
      return Scalar(1);
    }
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }
};

/**
 * sign() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> sign(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;

  // Evaluate constant
  if (x->type() == CONSTANT) {
    if (x->val < Scalar(0)) {
      return constant_ptr(Scalar(-1));
    } else if (x->val == Scalar(0)) {
      // Return zero
      return x;
    } else {
      return constant_ptr(Scalar(1));
    }
  }

  return make_expression_ptr<SignExpression<Scalar>>(x);
}

/**
 * Derived expression type for sin().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct SinExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr SinExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::sin;
    return sin(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::cos;
    return parent_adjoint * cos(x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * cos(x);
  }
};

/**
 * sin() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> sin(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::sin;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(sin(x->val));
  }

  return make_expression_ptr<SinExpression<Scalar>>(x);
}

/**
 * Derived expression type for sinh().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct SinhExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr SinhExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::sinh;
    return sinh(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::cosh;
    return parent_adjoint * cosh(x);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint * cosh(x);
  }
};

/**
 * sinh() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> sinh(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::sinh;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(sinh(x->val));
  }

  return make_expression_ptr<SinhExpression<Scalar>>(x);
}

/**
 * Derived expression type for sqrt().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct SqrtExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr SqrtExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::sqrt;
    return sqrt(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::sqrt;
    return parent_adjoint / (Scalar(2) * sqrt(x));
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    return parent_adjoint / (constant_ptr(Scalar(2)) * sqrt(x));
  }
};

/**
 * sqrt() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> sqrt(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::sqrt;

  // Evaluate constant
  if (x->type() == CONSTANT) {
    if (x->val == Scalar(0)) {
      // Return zero
      return x;
    } else if (x->val == Scalar(1)) {
      return x;
    } else {
      return constant_ptr(sqrt(x->val));
    }
  }

  return make_expression_ptr<SqrtExpression<Scalar>>(x);
}

/**
 * Derived expression type for tan().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct TanExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr TanExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::tan;
    return tan(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::cos;

    auto c = cos(x);
    return parent_adjoint / (c * c);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    auto c = cos(x);
    return parent_adjoint / (c * c);
  }
};

/**
 * tan() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> tan(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::tan;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(tan(x->val));
  }

  return make_expression_ptr<TanExpression<Scalar>>(x);
}

/**
 * Derived expression type for tanh().
 *
 * @tparam Scalar Scalar type.
 */
template <typename Scalar>
struct TanhExpression final : Expression<Scalar> {
  /**
   * Constructs an unary expression (an operator with one argument).
   *
   * @param lhs Unary operator's operand.
   */
  explicit constexpr TanhExpression(ExpressionPtr<Scalar> lhs)
      : Expression<Scalar>{std::move(lhs)} {}

  Scalar value(Scalar x, Scalar) const override {
    using std::tanh;
    return tanh(x);
  }

  ExpressionType type() const override { return ExpressionType::NONLINEAR; }

  Scalar grad_l(Scalar x, Scalar, Scalar parent_adjoint) const override {
    using std::cosh;

    auto c = cosh(x);
    return parent_adjoint / (c * c);
  }

  ExpressionPtr<Scalar> grad_expr_l(
      const ExpressionPtr<Scalar>& x, const ExpressionPtr<Scalar>&,
      const ExpressionPtr<Scalar>& parent_adjoint) const override {
    auto c = cosh(x);
    return parent_adjoint / (c * c);
  }
};

/**
 * tanh() for Expressions.
 *
 * @tparam Scalar Scalar type.
 * @param x The argument.
 */
template <typename Scalar>
ExpressionPtr<Scalar> tanh(const ExpressionPtr<Scalar>& x) {
  using enum ExpressionType;
  using std::tanh;

  // Prune expression
  if (x->is_constant(Scalar(0))) {
    // Return zero
    return x;
  }

  // Evaluate constant
  if (x->type() == CONSTANT) {
    return constant_ptr(tanh(x->val));
  }

  return make_expression_ptr<TanhExpression<Scalar>>(x);
}

}  // namespace slp::detail
